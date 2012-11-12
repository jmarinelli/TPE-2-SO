all:

	mkdir -p /tmp/cvs
	mkdir -p /tmp/cvs/config

	gcc -g -o /bin/cvsserver src/server/server.c src/filesystem/filesystem.c src/filesystem/dlist.c src/server/parser.c

	gcc -g -o /bin/cvs src/client/client.c
