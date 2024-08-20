#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "regex_check.h"
#include "recv_send.h"
#include "macro_funcs.h"

// returns heap allocated string with user nickname
// nickname must be freed by caller
char*
get_nickname()
{
	// size of nickname (255 chars) and \0 terminating byte
	const size_t max_nickname_size = 256;
	
	char *nickname = (char *)malloc(max_nickname_size * sizeof(char));

	// get user nickname
	fgets(nickname, max_nickname_size - 1, stdin);
	REM_NEWLINE(nickname, strlen(nickname));

	return nickname;
}

// strdup implementation to use -std=c99 standard
char*
strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
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

	const unsigned short port = (unsigned short)atoi(argv[2]);
	const char *ipv4_str = argv[1];

	char                *nickname;
	pthread_t           recv_th, send_th;
	struct recv_args    recv_args;
	struct send_args    send_args;

	printf("Welcome to IPv4 chat!\n");
	
	printf("Type your nickname [255 characters max]: ");
	nickname = get_nickname();

	printf("Now you can type your messages below (to stop type \"exit\")\n");

	recv_args.ipv4_str = strdup(ipv4_str);
	recv_args.port = port;

	send_args.nickname = strdup(nickname);
	send_args.port = port;

	pthread_create(&recv_th, NULL, recv_msg, (void *)&recv_args);
	pthread_create(&send_th, NULL, send_msg, (void *)&send_args);

	pthread_join(recv_th, NULL);
	pthread_join(send_th, NULL);
	
	free(nickname);
	free(recv_args.ipv4_str);
	free(send_args.nickname);

	return EXIT_SUCCESS;
}
