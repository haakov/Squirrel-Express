#include <pthread.h>
#include <ncurses.h>
#include <netdb.h>
#include "sqx.h"
#include <netinet/in.h>

/*
sqxclient.c - a tiny, simple chat client for use with the Squirrel Express chat server
Author: Håkon Vågsether <hauk142@gmail.com>
*/

void *stdRead(int *fd);
int sock, port; // TEH MAIN SOCKET
void cleanExit();
struct hostent *servur;                         /* The struct hostent pointer called servur */
struct sockaddr_in serverAddress;               /* The server's address, in a sockaddr_in struct */
char buf[20];


char closeMsg[] = "One of the clients has lost connection with the server.\n";
char servCloseMsg[] = "Server has disconnected, farewell world!\n";

int main(int argc, char *argv[]) // main function
{
	pthread_t stdRead_T; // thread (stdRead.c)

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
	// signal(SIGINT, cleanExit); // Ctrl+C calls the cleanExit function :33
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
	servur = gethostbyname(argv[1]);
	if(servur == NULL)
	{
		errorExit("gethostbyname()");   /* Everybody luvs error checking */
	}
    	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
    	bcopy((char *)servur->h_addr, (char *)&serverAddress.sin_addr.s_addr, servur->h_length);
	serverAddress.sin_port = htons(port = atoi(argv[2]));
    	if (connect(sock,(struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
	{
		errorExit("connect()");
	}

	//pthread_create(&stdRead_T, NULL, stdRead, &sock);

	fgets(buf, strlen(buf), stdin);
	write(sock, buf, sizeof(buf));

	//pthread_join(stdRead_T, NULL);
	cleanExit();                            /* Die and exit */
	return 0;                               /* You will never get here >:) */
}

	
	
void *stdRead(int *fd)
{
	char buffer[512];

	while (true) {
		fgets(buffer, sizeof(buffer), stdin); // receive user input
		write(fd, buffer, sizeof(buffer)); // write to server
	}
}


void cleanExit()
{
	close(sock);
	
	printf("Closing connection...\n");
	exit(EXIT_SUCCESS);
}
