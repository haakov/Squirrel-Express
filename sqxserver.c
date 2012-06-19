#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <strings.h>
#include "sqx.h"

#define MAXEVENTS 64
#define SRVPREFIX "srv"

/*
sqxserver.c - a tiny, simple chat server for use with the Squirrel Express chat client
Author: hauk142
*/

int sock, readSock, writeSock; // declare main socket
struct epoll_event *events; // the pointer epoll_event struct


void cleanExit();
void nonBlock(int fd); 

typedef struct // for communication between events[i].data.fd and the file descriptors to write to
{
	int in_use; // int to indicate if the integers below are in use
	int rFd; // events[i].data.fd
	int wFd; // file descriptor to write to
} fd;
fd fds[20];

int cliCount=0;

char servCloseMsg[] = "srv close";
char closeMsg[8]; // "srv 100" - 7 chars + one for \0
char helloMsg[12]; // "srv hey 100" - 11 chars + one for \0

int main(int argc, char *argv[]) // main function
{
	/* TODO: implement epoll */             /* stfu */
	/* TODO: Declaration sector starts here */
	int yes=1; // for Beej's "address in use"-loser
	int bindSock, portNum=33730, e; // socket for binding to the port. Also, lets have our portnumber as a variable, so we can change it later with the "-p" flag. An extra integer, e, for error checking
	int epfd; // the EPOLL file descriptor, and an integer for keeping track of how many clients we have connected, so it doesn't exceed our limit, which is currently 20
	struct sockaddr_in servAddr; // sockaddr_in struct for the main socket
	struct epoll_event event; // the normal epoll struct
	events = calloc(MAXEVENTS, sizeof(event));
	

	/* Declaration sector ends here */
	
	signal(SIGINT, cleanExit); // Ctrl+C calls the cleanExit function :33
	/* EPOLL sector starts here */
	epfd = epoll_create(20);
	if(epfd<0)
	{
		errorExit("epoll_create()");
	}
	/* EPOLL sector ends here */
	/* command-line arguments sector starts here */
	if(argc>1)
	{
		/* TODO add options for custom port */
		for(int i=1;i<argc;i++)
		{
			if(!strcmp(argv[i], "--help")||!strcmp(argv[i], "-h"))  // it is important to notice the "!" signs in front of the "strcmp"s
			{
				printf("Squirrel Express Server 0.5 alpha, a simple chat server.\n");
				printf("Developer may be contacted at hauk142 AT gmail DOT com.\n");
				printf("\n");
				printf("Options:\n");
				printf(" -h, --help \tdisplay this help and exit\n");
				printf(" -p, --port \trun on custom port (this feature is not available yet)\n");
				printf("\n");
				exit(EXIT_SUCCESS);
			}
			else
			{
				printf("Error: argument \"%s\" was not recognized. Try -h or --help.\n", argv[i]);
				exit(EXIT_FAILURE);
			}
		}
	}
	else
	{
		printf("Squirrel Express 0.5 alpha\n");
		printf("Exit with ^C or SIGINT.\n"); 
	}
	/* command-line arguments sector ends here */
	
	/* Create-and-bind sector starts here */
	
	sock = socket(AF_INET, SOCK_STREAM, 0); // TEH MASTER NINJA FILE DESCRIPTOR OMGZ
	if(sock<0) // we check for errors.
	{
		errorExit("sock()");
	}
        e = setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	/* binding foreplay dun dun dun dun duh-dun... */
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(portNum);

	/* the actual binding starts!! duh duh duuuhhh :O */
	bindSock = bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));
	if(bindSock<0) // we check for errors like my Android phone checks for new mail
	{
		errorExit("bind()");
	}
	else
	{
		printf("Successfully bound to %d\n", portNum);
	}
		

	/* make teh sockzet non-blocking */
	nonBlock(sock); // it has its own error handling :3

	e = listen(sock, SOMAXCONN); // we put this thing's ear at our sock
	if(e<0) // keep checkin' for them errors
	{
		errorExit("listen()");
	}
	
	/* EPOLL section again */
	event.data.fd = sock; 
	event.events = EPOLLIN | EPOLLET; // edge-triggered 

	e = epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event); // add the main socket to the list of file descriptors
	if(e<0) // check for errors
	{
		errorExit("epoll_ctl()");
	}
	
	/* lo and behold, the mighty while loop! */
	
	while(1)
	{
		e = epoll_wait(epfd, events, MAXEVENTS, -1); // the epoll_wait call
		for(int i=0;i<=cliCount;i++)
		{
			if(sock==events[i].data.fd && cliCount<=100) // we have activity on a listening socket, which means someone is trying to connect. Accept unless we're full
			{
				int acceptSock; // the file descriptor for each client
				struct sockaddr_in cliAddr; // the struct for the client address
				socklen_t cliLen; // the client address length socklen_t,
				cliLen = sizeof(cliAddr); // which, of course, is set to the size of cliAddr

				acceptSock = accept(sock, (struct sockaddr *) &cliAddr, &cliLen); // accept the client
				if(acceptSock<0) // we be checking errors
				{
					if(!((errno == EAGAIN) || (errno == EWOULDBLOCK))) // if the errors ARE NOT EAGAIN or EWOULDBLOCK
					{
						errorExit("accept()");
					}
				}

				nonBlock(acceptSock); // we do not want our accepted socket to block
				event.data.fd = acceptSock; // epoll_ctl foreplay
				event.events = EPOLLIN | EPOLLET; // epoll_ctl foreplay
				e = epoll_ctl(epfd, EPOLL_CTL_ADD, acceptSock, &event); // epoll_ctl, adding acceptSock to our epfd file descriptor
				if(e<0)
				{
					errorExit("epoll_ctl()");
				}
				for(int u=0;u<20;u++)
				{
					if(!fds[u].in_use) // notice the "!"! :D
					{
						fds[u].wFd = acceptSock;
						fds[u].rFd = events[i].data.fd;
						fds[u].in_use =1;
						break;
					}
				}
				cliCount++; // we consider the amount of clients to have increased
				sprintf(helloMsg, "%s hey %d", SRVPREFIX, cliCount); // Greet the client
				write( acceptSock, helloMsg, sizeof(helloMsg) );
				
		}
		else // data received on an accept()ed socket
			{
				if(events[i].data.fd>0)
				{
					while(1)
					{
						char buf[512];
						bzero(buf, sizeof(buf));
						readSock = read(events[i].data.fd, buf, sizeof(buf)); // read :D
						if(readSock == -1)
						{
							if(errno != EAGAIN)
							{
								errorExit("read()");
							}
							break;
						}
						if(readSock == 0)
						{
							close(events[i].data.fd); // close the file descriptor
							for(int u=0;u<20;u++)
							{
								if(fds[u].wFd == events[i].data.fd)
								{
									fds[u].in_use = 0;
								}
							}
							cliCount--; // one less file descriptor
							sprintf(closeMsg, "%s %d", SRVPREFIX, cliCount);
							for(int u=0;u<20;u++)
							{
								if(fds[u].in_use==1)
								{
									writeSock = write(fds[u].wFd, closeMsg, sizeof(closeMsg));
								}
							}
							break;
						}
						for(int u=0;u<20;u++)
						{
							if(fds[u].in_use)
							{
								writeSock = write(fds[u].wFd, buf, sizeof(buf));
							}
						}
					}
				}
			}
		}
	}
printf("The program ended. This is not supposed to happen.\n");
cleanExit();
return 0;
}



void nonBlock(int fd) // fd to set to nonblocking mode
{
	int flags, s;

	flags = fcntl(fd, F_GETFL, 0); // get the file descriptor's flags, set them to flags
	if(flags<0)
	{
		errorExit("fcntl()");
	}

	flags |= O_NONBLOCK; // add nonblocking to flags's flags... lol
	s=fcntl(fd, F_SETFL, flags); // set the new flags from flags to the file descriptor
	if(s<0) // and check for errors
	{
		errorExit("fcntl()");
	}
}


void cleanExit()
{
	
	for(int u=0;u<20;u++)
	{
		if(fds[u].in_use==1)
		{
			writeSock = write(fds[u].wFd, servCloseMsg, sizeof(servCloseMsg));
		}
	}
	/* send shutdown message to all of the clients */
	close(sock);
	free(events);
	for(int i=1;i<MAXEVENTS;i++)
	{
		close(events[i].data.fd);
	}
	
	
	printf("\nExiting!\n");
	exit(EXIT_SUCCESS);
}
