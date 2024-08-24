#include "recv_send.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "macro_funcs.h"

void
fill_sockaddr_in(struct sockaddr_in *addr,
                 const char *ipv4, const unsigned short port)
{
	// since inet_aton not set errno variable
	if (0 == inet_aton(ipv4, &(addr->sin_addr))) {
		fprintf(stderr, "inet_aton: IPv4 string is invalid!\n");
		exit(EXIT_FAILURE);
	}

	addr->sin_port = htons(port);
	addr->sin_family = AF_INET;
}

void*
recv_msg(void *arg)
{
	struct recv_args args = *((struct recv_args *)arg);

	int                   udp_socket;
	struct sockaddr_in    addr, src_addr;
	char                  msg[MAX_MSG_SIZE + 1];
	ssize_t               msg_size;
	char                  src_ipv4_str[INET_ADDRSTRLEN];
	socklen_t             src_addrlen;

	// creating socket
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == udp_socket)
		EHDLR("socket");
	
	// int broadcast_enable = 1;
	// setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable));

	// puts("UDP recieve socket was created successfully!");

	fill_sockaddr_in(&addr, BROADCAST_ADDR, args.port);
	// since inet_aton not set errno variable
	// if (0 == inet_aton(args.ipv4_str, &(addr.sin_addr))) {
	// 	fprintf(stderr, "inet_aton: IPv4 string is invalid!\n");
	// 	exit(EXIT_FAILURE);
	// }
	// addr.sin_addr.s_addr = INADDR_ANY;
	// addr.sin_port = htons(args.port);
	// addr.sin_family = AF_INET;


	if (-1 == bind(udp_socket, (struct sockaddr *)&addr, sizeof(addr)))
		EHDLR("bind");
	
	// puts("UDP recieve socket was binded successfully");
	// puts("Listening for incoming messages...\n\n");

	while (1) {
		msg_size = recvfrom(udp_socket, msg, MAX_MSG_SIZE - 1, MSG_WAITALL,
	                        (struct sockaddr *)&src_addr, &src_addrlen);
		if (-1 == msg_size)
			EHDLR("recvfrom");

		msg[msg_size] = '\0';

		if (!strcmp(msg, "exit"))
			break;

		inet_ntop(src_addr.sin_family, &src_addr.sin_addr, src_ipv4_str, INET_ADDRSTRLEN);
		printf("%s:%u\n", src_ipv4_str, ntohs(src_addr.sin_port));
		printf("%s\n\n", msg);

		usleep(5000);
	}

	close(udp_socket);

	/* fix later */
	return NULL;
}

void*
send_msg(void *arg)
{
	struct send_args args = *((struct send_args *)arg);

	int                   udp_socket;
	struct sockaddr_in    broadcast_addr;
	char                  msg[MAX_MSG_SIZE + sizeof("\0")];
	char                  full_msg[MAX_MSG_SIZE + strlen(args.nickname) + strlen(": ") + sizeof("\0")];
 
	fill_sockaddr_in(&broadcast_addr, BROADCAST_ADDR, args.port);

	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == udp_socket)
		EHDLR("socket");

	int broadcast_enable = 1;
	setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable));

	if (-1 == connect(udp_socket, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)))
		EHDLR("connect");

	while (1) {
		usleep(5000);
		
		printf("-> ");
		fgets(msg, MAX_MSG_SIZE, stdin);
		REM_NEWLINE(msg, strlen(msg));

		// add nickname to message
		sprintf(full_msg, "%s: %s", args.nickname, msg);

		if (-1 == sendto(udp_socket, full_msg, strlen(full_msg), 0,
		                 (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)))
			EHDLR("sendto");
		
		if (!strcmp(msg, "exit"))
			break;
	}

	close(udp_socket);

	/* fix later */
	return NULL;
}

