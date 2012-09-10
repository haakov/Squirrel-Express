#include <pthread.h>
#include <ncurses.h>
#include <netdb.h>
#include "sqx.h"
#include <netinet/in.h>

/*
sqxclient.c - a tiny, simple chat client for use with the Squirrel Express chat server
Author: Håkon Vågsether <hauk142@gmail.com>
*/

void *stdRead();
void cleanExit(int sentByThread);
int init_ncurses();

int sock, port; // TEH MAIN SOCKET
struct hostent *servur;                         /* The struct hostent pointer called servur */
struct sockaddr_in serverAddress;               /* The server's address, in a sockaddr_in struct */
char buf[512];
int cliCount; 		// number of clients connected to the server

pthread_t stdRead_T; 	// thread (stdRead.c)

char closeMsg[] = "One of the clients has lost connection with the server.\n";
char servCloseMsg[] = "Server has disconnected, farewell world!\n";

int main(int argc, char *argv[]) // main function
{
	/* {{{ Command-line arguments */ // TODO: Use getopt() instead!
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
    	memset( (char *) &serverAddress, 0, sizeof(serverAddress) );
	serverAddress.sin_family = AF_INET;
    	bcopy((char *)servur->h_addr, (char *)&serverAddress.sin_addr.s_addr, servur->h_length);
	serverAddress.sin_port = htons(port = atoi(argv[2]));
    	if (connect(sock,(struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
	{
		errorExit("connect()");
	}

	pthread_create(&stdRead_T, NULL, stdRead, NULL);
	init_ncurses();

	while(true)
	{
		getstr(buf); // TODO: Buffer overflow?
		if( !strncmp( buf, "/q", 2 ) || !strncmp( buf, "/quit", 5 ) )
		{
			cleanExit(0);
		}
		write(sock, buf, sizeof(buf));
		move(row-1, 0);
		clrtoeol();
		memset( buf, 0, sizeof(buf) );
	}

	cleanExit(0);                            /* Die and exit */
	return 0;                               /* You will never get here >:) */
}

	
	
void *stdRead()
{
	int n;
	char buffer[512];
	char *bufro;
	memset( buffer, 0, sizeof(buffer) );

	while (true) {
		n = read( sock, buffer, sizeof(buffer) );
		if(n<1)
			errorExit("read()");
		else
		{
			if( !strncmp( buffer, "msg", 3 ) ) // If someone said something
			{
				memset(buffer, 0, 3);
				bufro = buffer+4;
				wprintw(Wmessages, "%s\n", bufro);
				wrefresh(Wmessages);
				move(row-1, 0);
				refresh();
			}
			else // If a server message was received
			{
				if( !strncmp( buffer, "srv close", 9 ) ) // If the server is closing down
				{
					wattron(Wmessages, COLOR_PAIR(1));
					wprintw(Wmessages, "The server is closing down, Farewell World!\n"); 
					move(row-1, 0);
					wrefresh(Wmessages);
					wattroff(Wmessages, COLOR_PAIR(1));
					cleanExit(1);
				}
				else if( !strncmp( buffer, "srv hey", 7) ) // Okay these two else ifs are a bit sketchy, but they work
				{
					wattron(Wmessages, COLOR_PAIR(1));
//					wprintw(Wmessages, "Welcome to the server!\n");
					strtok(buffer, " ");
					strtok(NULL, " ");
					cliCount = atoi( strtok(NULL, " ") );
					if(cliCount==1)
						wprintw(Wmessages, "There is currently %d client connected.\n", cliCount); 
					else
						wprintw(Wmessages, "There are currently %d clients connected.\n", cliCount); 
					wrefresh(Wmessages);
					wattroff(Wmessages, COLOR_PAIR(1));
				}
				else if( !strncmp( buffer, "srv", 3) ) // Someone has joined/left
				{
					wattron(Wmessages, COLOR_PAIR(1));
					strtok(buffer, " ");
					cliCount = atoi( strtok(NULL, " ") );
					if(cliCount==1)
						wprintw(Wmessages, "There is currently %d client connected.\n", cliCount); 
					else
						wprintw(Wmessages, "There are currently %d clients connected.\n", cliCount); 
					wrefresh(Wmessages);
					wattroff(Wmessages, COLOR_PAIR(1));
				}

			}
			memset( buffer, 0, sizeof(buffer) );
		}
	}
	return 0;
}


void cleanExit(int sentByThread)
{
	endwin();
	if(!sentByThread)
			pthread_cancel(stdRead_T);
	close(sock);
	

	printf("Closing connection...\n");
	exit(EXIT_SUCCESS);
}

int init_ncurses()
{
	initscr();
	getmaxyx(stdscr, row, col);
	if(has_colors() == FALSE)
	{
		puts("has_colors() returned FALSE!\n");
		return 1;
	}
	start_color();
	init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
	move(row-1, 0);
	Wmessages=newwin(row-3, col, 1, 0);
	idlok(Wmessages, TRUE);
	scrollok(Wmessages, TRUE);
	return 0;
}
