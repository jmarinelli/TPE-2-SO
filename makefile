all:

	rm -r -f bin
	mkdir bin

	gcc -g -o bin/cvsserver src/server/server.c src/filesystem/dlist.c src/server/parser.c

	gcc -g -o bin/cvsclient src/client/client.c
