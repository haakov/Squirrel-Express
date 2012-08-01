server : sqxserver.c sqx.h
	gcc -Wall -std=gnu99 -g sqxserver.c -o sqxs -pthread -DSQX_SERVER

client : sqxclient.c sqx.h
	gcc -Wall  -std=gnu99 -g sqxclient.c -o sqxc -pthread -lncurses -DSQX_CLIENT

all : sqxserver.c sqx.h sqxclient.c
	gcc -Wall -std=gnu99 -g sqxserver.c -o sqxs -pthread -DSQX_SERVER
	gcc -Wall  -std=gnu99 -g sqxclient.c -o sqxc -pthread -lncurses -DSQX_CLIENT

clean:
	rm sqxc sqxs
