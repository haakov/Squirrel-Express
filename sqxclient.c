#include <pthread.h>
#include <ncurses.h>
#include "sqx.h"

/*
sqxclient.c - a tiny, simple chat client for use with the Squirrel Express chat server
Author: Håkon Vågsether
*/

void *stdRead(int *fd);
void cleanExit();

int main(int argc, char *argv[]) // main function
{
	/* {{{ Declarations */
	int sock; // TEH MAIN SOCKET
	pthread_t stdRead_T; // thread (stdRead.c)
	char username[32];
	/* {{{ */

	/* {{{ Command-line arguments */
	if (!(strcmp(argv[1], "--help")||strcmp(argv[1], "-h"))) {
		printf("Squirrel Express client 0.1 pre-alpha, a simple chat client for use with the Squirrel Express server.\n");
		printf("This program has so far only been tested on Arch Linux 32 bit.\n");
		printf("Developer can be contacted at hauk142 AT users DOT sf DOT net.\n");
		printf("\n");
		printf("Usage: sqxc <options> <IP address> <port number>\n");
		printf("Options:\n");
		printf(" -h, --help \tdisplay this help and exit\n");
		printf("\n");
		exit(EXIT_SUCCESS);
	}
	else if (argc!=3) {
		printf("Usage: sqxc <options> <IP address> <port number>\n");
		exit(EXIT_FAILURE);
	}
	/* }}} */
	signal(SIGINT, cleanExit); // Ctrl+C calls the cleanExit function :33
/*	
	This is supposed to be anonymous, right?
	printf("Pick a username: "); // prompt for username
	fgets(username, sizeof(username), stdin); 
*/	

	/* LET THERE BE BINDING */
	sock = socket(AF_INET, SOCK_STREAM, 0); // initiate main socket
	/* errorcheck */
	if (sock<0) {
		errorExit("socket()");
	}

	cleanExit();	
	return 0;
}

	
	
void *stdRead(int *fd)
{
	char buffer[512];

	while (true) {
		fgets(buffer, sizeof(buffer), stdin); // receive user input
		/* TODO: Add nick to message? */
		write(fd, buffer, sizeof(buffer)); // write to server
	}
}


void cleanExit()
{
	close(sock);
	
	printf("\nExiting!\n");
	exit(EXIT_SUCCESS);
}
