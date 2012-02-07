#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

/* 
sqx.h - a header file for the squirrel express server/client
Author: hauk142
*/

int errorExit(char *failDesc) // function for exiting on error
{
	perror("failDesc");
	exit(EXIT_FAILURE);
}

