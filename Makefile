server : sqxserver.c sqx.h
	gcc -Wall -std=c99 -g sqxserver.c -o sqxs

client : sqxclient.c sqx.h
	gcc -Wall  -std=c99 -g sqxclient.c -o sqxc

clean:
	rm sqxc sqxs
