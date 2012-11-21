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

fstree_node_t find_child_by_path(fstree_node_t node, string path) {
	fslist_node_t aux_node = node->children->first;
	fstree_node_t ret;
	while (aux_node) {
		ret = aux_node->child;
		if (!strcmp(ret->filename, path))
			return ret;
		aux_node = aux_node->next;
	}
	return NULL;
}

fstree_node_t add_node_if_not_existing(fstree_node_t node, string path, bool is_dir){
	fstree_node_t aux_node = find_child_by_path(node, path);
	if (!aux_node) {
		aux_node = new_fstree_node(is_dir, path, 0); /* Despues vemos el inodo */
		add_child(node, aux_node); 
	}
	return aux_node;
}

error update_child_from_path(fstree_t tree, string path) {
	char * position;
	char * aux = calloc(1, MAX_PATH_LENGTH);
	fstree_node_t current_node = tree->root;
	strcpy(aux, path);
	
	while(position = strchr(aux, '/')) {
		*position = 0;
		current_node = add_node_if_not_existing(current_node, aux, TRUE);
		aux = position+1;
	}
	
	if (!add_node_if_not_existing(current_node, aux, TRUE))
			return NO_MEMORY;
	return SUCCESS;
}

fstree_node_t nodecpy ( fstree_node_t node_to_cpy ) {
	return new_fstree_node(node_to_cpy->is_directory, 
					node_to_cpy->filename, node_to_cpy->server_inode);
}

fstree_node_t treecpy ( fstree_node_t old_node ) {
	fslist_node_t aux_node;
	fstree_node_t new_node;
	if (!old_node->children->size)
		return nodecpy(old_node);
	new_node = nodecpy(old_node);
	aux_node = old_node->children->first;
	while (aux_node != NULL) {
		add_child(new_node, treecpy(aux_node->child));
		aux_node = aux_node->next;
	}
	return new_node;
}

fstree_t branch_tree ( fstree_t old_tree ) {
	fstree_t new_tree = new_fstree();
	fstree_node_t new_root = treecpy(old_tree->root);
	new_tree->root = new_root;
	return new_tree;
}
