/* General */

typedef char * 				string;
typedef unsigned			bool;
typedef int					error;

/* Double linked list */

typedef dlist_node * 		dlist_node_t;
typedef dlist * 			dlist_t;

/* File-system tree */

typedef fstree * 			fstree_t;
typedef fstree_node * 		fstree_node_t;
typedef fslist *			fslist_t;
typedef fslist_node *		fslist_node_t;

/* Parser */

#define CLEAN_LINE int i;for(i=0;i<MAX_INSTRUCTION_LENGTH;i++)line[i]=0;
#define CANT_INSTRUCTIONS 	6
#define MAX_NAME_SIZE 		200
#define CVS_LENGTH 			4
#define TRUE				1
#define FALSE				!TRUE;

/* Errors */

#define SUCCESS				 0
#define NON_EXISTING_PARENT	-1
#define NO_DIRECTORY		-2
#define NO_MEMORY			-3

/* Server */

#define SERVER_CHANNEL "/tmp/server"
