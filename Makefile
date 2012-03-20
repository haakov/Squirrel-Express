server : sqxserver.c sqx.h
	gcc -Wall -std=c99  sqxserver.c -o sqxs

client : sqxclient.c sqx.h
	gcc -Wall  -std=c99 sqxclient.c -o sqxc

clean:
	rm sqxc sqxs
