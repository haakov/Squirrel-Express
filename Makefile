server : sqxserver.c sqx.h
	gcc -Wall -std=gnu99 -g sqxserver.c -o sqxs -pthread

client : sqxclient.c sqx.h
	gcc -Wall  -std=gnu99 -g sqxclient.c -o sqxc -pthread

all : sqxserver.c sqx.h sqxclient.c
	gcc -Wall -std=gnu99 -g sqxserver.c -o sqxs -pthread
	gcc -Wall  -std=gnu99 -g sqxclient.c -o sqxc -pthread

clean:
	rm sqxc sqxs
