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

/* IPC Headers */

typedef instruction_header * instruction_header_t;

/* Parser */

#define CLEAN_LINE int i;for(i=0;i<MAX_INSTRUCTION_LENGTH;i++)line[i]=0;
#define CANT_INSTRUCTIONS 	6
#define MAX_NAME_SIZE 		200
#define CVS_LENGTH 			4
#define TRUE				1
#define FALSE				!TRUE

/* Errors */

#define SUCCESS				 0
#define NON_EXISTING_PARENT	-1
#define NO_DIRECTORY		-2
#define NO_MEMORY			-3
#define NON_EXISTING_FILE	-4

/* Paths */

#define REPOSITORY_PATH 	"/usr/share/cvs"
#define SERVER_CHANNEL 		"/tmp/server"
#define CVS_TREE_PATH 		"/tmp/cvs/config/tree"
#define MAX_PATH_LENGTH		1024

/* System Tree */

#define CVS_ROOT_NAME 		"root"
#define IS_FILE 			1
#define IS_DIRECTORY 		2

/* Tree status */

#define NO_STATE		1	 
#define	UPDATED 		2
#define	ADDED 			3
#define	DELETED 		4
#define	INSIDE_CHANGED 	5
