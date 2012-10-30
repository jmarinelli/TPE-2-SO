#include <stdlib.c>
#include "../include/structs.h"
#include "../include/defs.h"

dlist_t new_dlist() {
	dlist_t new_list = (dlist_t)calloc(sizeof(dlist), 1);
	return new_list;
}

int add(void * data, size_t data_size, dlist_t current) {
	dlist_node_t aux;
	if ( current == NULL )
		return -1;
	if ( current->first == NULL ) {
		if ( current->first = new_dlist_node(data, data_size) == NULL)
			return -1;
		current->current = current->first;
		return 0;
	}
	aux = current->first;
	while ( aux->next != NULL )
		aux = aux->next;
	if ( aux->next = new_dlist_node(data, data_size) == NULL )
		return -1;
	aux->next->prev = aux;
	return 0;
}

dlist_node_t new_dlist_node(void * data, size_t data_size) {
	dlist_node_t new_node = (dlist_node_t)calloc(sizeof(dlist_node), 1);
	if ( new_node == NULL )
		return NULL;
	new_node->data = data;
	new_node->data_size = data_size;
	return new_node;
}
