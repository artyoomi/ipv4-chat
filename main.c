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

#define IPV4_REG \
"^((25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])$"

#define PORT_REG \
"^(6553[0-6]|655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[0-5]?[0-9]{0,4})$"

#define REM_NEWLINE(msg, len) { if (len > 0 && '\n' == msg[len - 1]) msg[len - 1] = '\0'; }
#define EHDLR(msg) { perror(msg); exit(EXIT_FAILURE); }

char           *ip_str;
unsigned short port;
char           *nickname;

// returns heap allocated string with user nickname
// nickname must be freed by caller
char*
get_nickname()
{
	char *nickname = (char *)malloc(MAX_NNAME_SIZE * sizeof(char));

	// get user nickname
	fgets(nickname, MAX_NNAME_SIZE, stdin);
	REM_NEWLINE(nickname, strlen(nickname));

	return nickname;
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
	
	int recv_socket = socket(PF_INET, SOCK_DGRAM, 0);
	if (-1 == recv_socket)
		EHDLR("socket");
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	// if (0 == inet_pton(AF_INET, ip_str, &(addr.sin_addr))) {
	// 	fprintf(stderr, "Error: IPv4 address is invalid!\n");
	// 	exit(EXIT_FAILURE);
	// }
	
	if (-1 == bind(recv_socket, (struct sockaddr *)&addr, sizeof(addr)))
		EHDLR("bind");

	ssize_t            msglen;
	struct sockaddr_in sender_addr;
	socklen_t          sender_addr_len;

	memset(&addr, 0, sizeof(addr));
	
	char sender_ip_str[INET_ADDRSTRLEN];
	
	while (true) {
		msglen = recvfrom(recv_socket, msg, MAX_MSG_SIZE - 1, 0,
		                  (struct sockaddr *)&sender_addr,
		                  &sender_addr_len);
		if (-1 == msglen)
			EHDLR("recvfrom");

		msg[msglen] = '\0';

		// check for exit expression
		if (!strcmp(msg, "exit"))
			break;;

		// get ip and port and print them with message
		if (NULL == inet_ntop(AF_INET, &sender_addr,
			                  sender_ip_str, INET_ADDRSTRLEN))
			EHDLR("inet_ntop");
			
		printf(" IP: [%s] Port: [%hu]\n %s\n",
			   sender_ip_str, ntohs(sender_addr.sin_port), msg);
	}

	close(recv_socket);
	
	return NULL;
}

void*
send_msg(void *arg)
{
	// create socket to send information
	int send_socket = socket(PF_INET, SOCK_DGRAM, 0);
	if (-1 == send_socket)
		EHDLR("socket");

	// make socket broadcast
	int so_bcast = 1;
	if (-1 == setsockopt(send_socket, SOL_SOCKET, SO_BROADCAST, &so_bcast,
	           			 sizeof so_bcast))
		EHDLR("setsockopt");

	const char *ip_bcast_str = "255.255.255.255";

	// fill sockaddr_in struct to store ip and port
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	// since inet_aton isnt set errno value
	if (0 == inet_pton(AF_INET, ip_bcast_str, &(addr.sin_addr))) {
		fprintf(stderr, "Error: IPv4 address is invalid!\n");
		exit(EXIT_FAILURE);
	}

	// just to msg size
	char msg[MAX_MSG_SIZE];
	// to template like <nickname>: <message>
	char msgtosend[MAX_MSG_SIZE + MAX_NNAME_SIZE + 2];

	// sending msges
	ssize_t sendmsglen;
	while (true) {
		fgets(msg, MAX_MSG_SIZE, stdin);
		REM_NEWLINE(msg, strlen(msg));

		sprintf(msgtosend, "%s: %s", nickname, msg);
		
		sendmsglen = sendto(send_socket, msgtosend, strlen(msgtosend), 0,
		                    (struct sockaddr *)&addr, sizeof(addr));
	
		if (!strcmp(msg, "exit"))
			break;
	}

	close(send_socket);
	
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

	// send global variables ip_str and port
	ip_str = argv[1];
	port = (unsigned short)atoi(argv[2]);

	pthread_t recv_th, send_th;

	printf("Welcome to IPv4 chat!\n");

	// set global variable nick
	printf("Type your nickname [255 characters max]: ");
	nickname = get_nickname();

	printf("\nNow you can type your messages below (to stop type \"exit\")\n");

	pthread_create(&recv_th, NULL, recv_msg, NULL);
	pthread_create(&send_th, NULL, send_msg, NULL);

	pthread_join(recv_th, NULL);
	pthread_join(send_th, NULL);
	
	free(nickname);

	return EXIT_SUCCESS;
}
