typedef struct dlist_node {
	void * data;
	size_t data_size;
	struct dlist_node * previous;
	struct dlist_node * next;
} dlist_node;

typedef struct dlist {
	dlist_node * first;
	dlist_node * current; // (?)
	int size;
} dlist;
