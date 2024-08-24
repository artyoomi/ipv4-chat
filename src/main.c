#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MSG_SIZE   1000
#define MAX_NNAME_SIZE 50

#define BCAST_IP "255.255.255.255"

#define IPV4_REG \
"^((25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])$"
#define PORT_REG \
"^(6553[0-6]|655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[0-5]?[0-9]{0,4})$"

#define REM_NEWLINE(msg, len) \
{ if (len > 0 && '\n' == msg[len - 1]) msg[len - 1] = '\0'; }
#define EHDLR(msg) \
{ perror(msg); exit(EXIT_FAILURE); }

// set necessary global variables
int            sockfd;
char           nickname[MAX_NNAME_SIZE];
char           ip_str[INET_ADDRSTRLEN];
unsigned short port;

int
socket_init()
{
	int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if (-1 == sockfd)
		EHDLR("main: socket");
	
	// make socket broadcast
	int so_bcast = 1;
	if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &so_bcast,
	           			 sizeof so_bcast))
		EHDLR("sender: setsockopt");

	struct sockaddr_in recv_addr;

	// fill recv_addr
	memset(&recv_addr, 0, sizeof(recv_addr));
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_port = htons(port);
	// addr.sin_addr.s_addr = INADDR_ANY;
	if (-1 == inet_pton(AF_INET, ip_str, &(recv_addr.sin_addr)))
		EHDLR("socket_init: inet_pton");
	
	if (-1 == bind(sockfd, (struct sockaddr *)&recv_addr, sizeof(recv_addr)))
		EHDLR("main: bind");
	
	return sockfd;
}

// check if string match to specific template
bool
correctness_check(const char *str, const char *regstr)
{
	int        stat;           // regexec return status
	regex_t    regex;          // regex struct
	char       errmsg[255];    // regexec error msg
	
	if (regcomp(&regex, regstr, REG_EXTENDED))
		EHDLR("regcomp");

	stat = regexec(&regex, str, 0, NULL, 0);
	if (0 == stat) {
		regfree(&regex);
		return true;
	} else if (REG_NOMATCH == stat) {
		regfree(&regex);
		return false;
	} else {
		regerror(stat, &regex, errmsg, sizeof(errmsg));
		regfree(&regex);

		fprintf(stderr, "Regex match failed: %s\n", errmsg);
		exit(EXIT_FAILURE);
	}
}

void*
recv_msg(void *arg)
{
	// to template like <nickname>: <message>
	char msg[MAX_MSG_SIZE + MAX_NNAME_SIZE + 2];
	
	ssize_t            msglen;
	struct sockaddr_in sender_addr;
	socklen_t          sender_addr_len = sizeof(sender_addr);

	char sender_ip_str[INET_ADDRSTRLEN];
	
	// to size of ": exit" + sizeof nickname
	char exit_expr[strlen(nickname) + 6];
	sprintf(exit_expr, "%s: exit", nickname);
	
	while (true) {
		msglen = recvfrom(sockfd, msg, MAX_MSG_SIZE - 1, 0,
		                  (struct sockaddr *)&sender_addr,
		                  &sender_addr_len);
		if (-1 == msglen)
			EHDLR("recv_msg: recvfrom");

		msg[msglen] = '\0';

		// get ip string
		if (NULL == inet_ntop(AF_INET, &(sender_addr.sin_addr),
			                  sender_ip_str, INET_ADDRSTRLEN))
			EHDLR("recv_msg: inet_ntop");
		
		if (!strcmp(msg, exit_expr))
			break;
		
		printf(" IP: [%s] Port: [%hu]\n %s\n",
		   	   sender_ip_str, ntohs(sender_addr.sin_port), msg);
	}

	return NULL;
}

void*
send_msg(void *arg)
{
	struct sockaddr_in bcast_addr;
	
	// fill sockaddr_in struct to store ip and port
	memset(&bcast_addr, 0, sizeof(bcast_addr));
	bcast_addr.sin_family = AF_INET;
	bcast_addr.sin_port = htons(port);
	if (-1 == inet_pton(AF_INET, BCAST_IP, &(bcast_addr.sin_addr)))
		EHDLR("send_msg: inet_pton");

	
	
	// just to msg size
	char msg_content[MAX_MSG_SIZE];
	// to template like <nickname>: <message>
	char msg[MAX_MSG_SIZE + MAX_NNAME_SIZE + 2];

	ssize_t sendmsglen;

	// to template like: <nickname> was joined in chat!
	char hello_msg[strlen(nickname) + strlen(" was joined in chat!")];
	sprintf(hello_msg, "%s %s", nickname, "was joined in chat!");
	
	// hello message to chat
	sendmsglen = sendto(sockfd, hello_msg, strlen(hello_msg), 0,
	                    (struct sockaddr *)&bcast_addr, sizeof(bcast_addr));
	if (-1 == sendmsglen)
		EHDLR("send_msg: hello sendto");
	
	while (true) {
		fgets(msg_content, MAX_MSG_SIZE, stdin);
		REM_NEWLINE(msg_content, strlen(msg_content));

		sprintf(msg, "%s: %s", nickname, msg_content);
		
		sendmsglen = sendto(sockfd, msg, strlen(msg), 0,
		                    (struct sockaddr *)&bcast_addr, sizeof(bcast_addr));
		if (-1 == sendmsglen)
			EHDLR("sender: sendto");
	
		if (!strcmp(msg_content, "exit"))
			break;
	}
	
	return NULL;
}

int
main(int argc, char *argv[])
{
	if (3 != argc) {
		fprintf(stderr, "Usage: %s <IPv4> <Port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	// check correctness of ipv4 string
	if (!correctness_check(argv[1], IPV4_REG)) {
		fprintf(stderr, "Error: Invalid IPv4 address!\n");
		return EXIT_FAILURE;
	}

	// check port correctness (must be 0..65535)
	if (!correctness_check(argv[2], PORT_REG)) {
		fprintf(stderr, "Error: Invalid Port!\n");
		return EXIT_FAILURE;
	}

	// set global variables ip_str and port
	strncpy(ip_str, argv[1], INET_ADDRSTRLEN);
	port = (unsigned short)atoi(argv[2]);

	// init broadcast socket
	sockfd = socket_init();
	
	printf("Welcome to IPv4 chat!\n");

	// set nickname global variable
	printf("Type your nickname [255 characters max]: ");
	fgets(nickname, MAX_NNAME_SIZE, stdin);
	REM_NEWLINE(nickname, strlen(nickname));

	printf("\nNow you can type your messages below (to stop type \"exit\")\n\n");

	pthread_t recv_th, send_th;
	
	pthread_create(&recv_th, NULL, recv_msg, NULL);
	pthread_create(&send_th, NULL, send_msg, NULL);

	pthread_join(recv_th, NULL);
	pthread_join(send_th, NULL);
	
	close(sockfd);

	return EXIT_SUCCESS;
}
