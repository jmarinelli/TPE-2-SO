#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/structs.h"
#include "../../include/defs.h"

dlist_node_t new_dlist_node(void *, size_t);

dlist_t new_dlist() {
	dlist_t new_list = (dlist_t)calloc(sizeof(dlist), 1);
	return new_list;
}

int add(void * data, size_t data_size, dlist_t list) {
	dlist_node_t aux;
	if ( list == NULL )
		return -1;
	if ( list->first == NULL ) {
		if ( (list->first = new_dlist_node(data, data_size)) == NULL)
			return -1;
		list->last = list->first;
		list->size = 1;
		return 0;
	}
	aux = list->last;
	if ( (aux->next = new_dlist_node(data, data_size)) == NULL )
		return -1;
	aux->next->previous = aux;
	list->last = aux->next;
	list->size++;
	return 0;
}

dlist_node_t new_dlist_node(void * data, size_t data_size) {
	dlist_node_t new_node = (dlist_node_t)calloc(sizeof(dlist_node), 1);
	if ( new_node == NULL )
		return NULL;
	new_node->data = malloc( data_size );
	memcpy( new_node->data , data, data_size );
	new_node->data_size = data_size;
	return new_node;
}
