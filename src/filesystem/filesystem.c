#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/filesystem/fstree.h"

void retrieve_tree(char * path , fstree_node_t node);
fstree_t filesystem_init();

fstree_t filesystem_init() {
	 fstree_t tree = new_fstree();
	 retrieve_tree(REPOSITORY_PATH, tree->root);
	 return tree;
}

bool tree_contains(fstree_node_t node, char * path) {
	fslist_node_t aux_node = node->children->first;
	while (aux_node != NULL) {
		if (!strcmp(aux_node->child->filename, path))
			return TRUE;
		aux_node = aux_node->next;
	}
	return FALSE;
}

void retrieve_tree(char * path , fstree_node_t node) {
	DIR * dir_path = opendir(path);
	fstree_node_t new_child;
	bool is_dir = FALSE;
	bool already_read = FALSE;
	struct dirent entry;
	struct dirent * result;
	char new_path[MAX_PATH_LENGTH];
	do {
		readdir_r(dir_path, &entry, &result);
		if ( strncmp(entry.d_name, ".", 1) && strncmp(entry.d_name, "..", 2) ) {
			if (!tree_contains(node, entry.d_name)) {
				strcpy( new_path , path );
				strcat( new_path , "/" );
				strcat( new_path , entry.d_name );
				if( entry.d_type == DT_DIR ) {
					is_dir = TRUE;
				}
				new_child = new_fstree_node( is_dir , entry.d_name , entry.d_ino);
				add_child( node , new_child );
				if ( is_dir ) 
					retrieve_tree( new_path , new_child );
				is_dir = FALSE;
			}
		}
	} while ( result != NULL );
}
