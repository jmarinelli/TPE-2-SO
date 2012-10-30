all:

	rm -r -f bin
	mkdir bin

	gcc -g -o bin/cvs src/filesystem/dlist.c src/server/parser.c
