#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/filesystem/fstree.h"

void retrieve_tree(char * path , fstree_node_t node);
fstree_t filesystem_init();

fstree_t filesystem_init() {
	
	/*
	 * TODO: 
	 * 
	 * - Dentro de la carpeta cvs busca la carpeta config con el archivo
	 * 	 con el arbol guardado y lo levanta a memoria, si no esta lo 
	 *   crea.
	 * - 
	 * 
	 * */
	 fstree_t tree = new_fstree();
	 retrieve_tree(SERVER_CHANNEL, tree->root);
	 return tree;
}

void retrieve_tree(char * path , fstree_node_t node) {
	DIR * dir_path = opendir(path);
	fstree_node_t new_child;
	bool is_dir = FALSE;
	struct dirent entry;
	struct dirent * result;
	char new_path[256];
	do {
		readdir_r(dir_path, &entry, &result);
		if ( strncmp(entry.d_name, ".", 1) && strncmp(entry.d_name, "..", 2) ) {
			if( entry.d_type == DT_DIR ) {
				is_dir = TRUE;
				strcpy( new_path , path );
				strcat( new_path , "/" );
				strcat( new_path , entry.d_name );
			}
			new_child = new_fstree_node( is_dir , entry.d_name , entry.d_ino);
			add_child( node , new_child );
			if ( is_dir )
				retrieve_tree( new_path , new_child );
			is_dir = FALSE;
		}
	} while ( result != NULL );
}
