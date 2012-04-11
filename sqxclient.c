#include <pthread.h>
#include <ncurses.h>
#include "sqx.h"

/*
sqxclient.c - a tiny, simple chat client for use with the Squirrel Express chat server
Author: Håkon Vågsether <hauk142@gmail.com>
*/

void *stdRead(int *fd);
int sock; // TEH MAIN SOCKET
void cleanExit();
struct hostent *servur;                         /* The struct hostent pointer called servur */

int main(int argc, char *argv[]) // main function
{
	/* {{{ Declarations */
	pthread_t stdRead_T; // thread (stdRead.c)
	/* {{{ */

	/* {{{ Command-line arguments */
	if (argc<1 && (!(strcmp(argv[1], "--help")||strcmp(argv[1], "-h")))) {
		printf("Squirrel Express Client, a simple chat client for use with the Squirrel Express server.\n");
		printf("Developer can be contacted at hauk142 AT gmail DOT com.\n");
		printf("\n");
		printf("Usage: sqxc <options> <IP address> <port number>\n");
		printf("Options:\n");
		printf(" -h, --help \tdisplay this help and exit\n");
		printf("\n");
		exit(EXIT_SUCCESS);
	}
	else if (argc!=3) {
		printf("Usage: sqxc <IP address> <port number> <options>\n");
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


	cleanExit();                            /* Die and exit */
	return 0;                               /* You will never get here >:) */
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
