/* Double linked list */

typedef struct dlist_node {
	void * data;
	size_t data_size;
	struct dlist_node * previous;
	struct dlist_node * next;
} dlist_node;

typedef struct dlist {
	dlist_node * first;
	dlist_node * last;
	int size;
} dlist;

/* File-system Tree */

typedef struct fslist_node {
	struct fstree_node * child;
	struct fslist_node * next;
} fslist_node;

typedef struct fslist {
	struct fslist_node * first;
	size_t size;
} fslist;

typedef struct fstree_node {
	fslist * children;
	unsigned is_directory;
	dlist * versions;		// sin uso APARENTE por ahora
	char * filename;
} fstree_node;

typedef struct fstree {
	struct fstree_node * root;
} fstree;

/* IPC Headers */

typedef struct instruction_header {
	int instruction_size;
	int client_id;
} instruction_header;
