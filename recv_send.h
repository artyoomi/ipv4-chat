#ifndef RECV_SEND_H
#define RECV_SEND_H

#include <arpa/inet.h>

#define MAX_MSG_SIZE   1000
#define BROADCAST_ADDR "255.255.255.255"

struct recv_args {
	char *ipv4_str;
	unsigned short port;
};

struct send_args {
	char *nickname;
	unsigned short port;
};

void
fill_sockaddr_in(struct sockaddr_in *addr,
                 const char *ipv4, const unsigned short port);

void*
recv_msg(void *arg);

void*
send_msg(void *arg);
#endif
