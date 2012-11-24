/* Double linked list */

typedef struct dlist_node {
	void * data;
	unsigned data_size;
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
	unsigned size;
} fslist;

typedef struct fstree_node {
	fslist * children;
	unsigned is_directory;
	dlist * versions;		// sin uso APARENTE por ahora
	unsigned server_inode;
	unsigned client_inode;
	int status;
	char * filename;
} fstree_node;

typedef struct fstree {
	struct fstree_node * root;
	int tree_id;
} fstree;

/* IPC Headers */

typedef struct instruction_header {
	unsigned instruction_size;
	unsigned current_path_size;
	unsigned parameter_size;
	unsigned parameter_size2;
	int client_id;
} instruction_header;

