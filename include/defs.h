/* General */

typedef char * string;

/* Double linked list */

typedef dlist_node * dlist_node_t;
typedef dlist * dlist_t;

/* Parser */

#define CLEAN_LINE int i;for(i=0;i<MAX_INSTRUCTION_LENGTH;i++)line[i]=0;
#define CANT_INSTRUCTIONS 6
#define MAX_NAME_SIZE 200
#define CVS_LENGTH 4
