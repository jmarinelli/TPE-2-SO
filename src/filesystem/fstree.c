#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/filesystem/dlist.h"

fslist_t new_fslist ( void ) {
	return ( fslist_t )calloc( sizeof( fslist ) , 1 );
}

fstree_node_t new_fstree_node ( bool is_directory , string filename , unsigned inode ) {
	fstree_node_t node = 
				( fstree_node_t )calloc( sizeof( fstree_node ) , 1 );
	if ( node == NULL )
		return NULL;
	node->filename = calloc( sizeof(char) , strlen(filename) + 1 );
	memcpy(node->filename, filename, strlen(filename) + 1);
	node->is_directory = is_directory;
	node->versions = new_dlist(); // (?)
	node->server_inode = inode;
	if ( ( node->children = new_fslist() ) == NULL )
		return NULL;
	return node;
}

fstree_t new_fstree ( void ) {
	fstree_t tree = ( fstree_t )calloc( sizeof( fstree ) , 1 );
	if ( tree == NULL )
		return NULL;
	if ( ( tree->root = new_fstree_node( TRUE, CVS_ROOT_NAME, 0 ) ) == NULL )
		return NULL;
	return tree;
}

error add_fslist( fslist_t list , fstree_node_t node_to_add ) {
	fslist_node_t aux_node = list->first;
	fslist_node_t new_node = 
				( fslist_node_t )calloc( sizeof( fslist_node ) , 1 );
	if ( new_node == NULL )
		return NO_MEMORY;
	new_node->child = node_to_add;
	new_node->next = aux_node;
	list->first = new_node;
	list->size++;
	return SUCCESS;
}

error add_child ( fstree_node_t parent , fstree_node_t child ) {
	if ( parent == NULL )
		return NON_EXISTING_PARENT;
	if ( !parent->is_directory )
		return NO_DIRECTORY;
	return add_fslist( parent->children , child );
}
