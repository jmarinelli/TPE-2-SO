all:

	mkdir -p /usr/share/cvs
	mkdir -p /usr/share/cvs/old

	gcc -g -o /bin/cvsserver src/server/server.c src/filesystem/filesystem.c src/filesystem/dlist.c src/filesystem/fstree.c src/cvs/functions.c src/cvs/versions.c src/cvs/utils.c src/server/parser.c

	gcc -g -o /bin/cvs src/client/client.c
