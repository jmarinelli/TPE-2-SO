#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/structs.h"
#include "../../include/defs.h"

extern dlist_t tree_list;
extern fstree_t repository_tree;

fstree_t get_client_tree(int client_id){
	dlist_node_t aux_node = tree_list->first;
	fstree_t ans;
	
	while(aux_node != NULL) {
		ans = (fstree_t)aux_node->data;
		if (ans->tree_id == client_id)
			return ans;
		aux_node = aux_node->next;
	}
	
	ans = (fstree_t)branch_tree(repository_tree);
	dlist_add(ans, sizeof(fstree), tree_list);
	return ans;
}

bool check_existing_file (char* path, char*file) {
	return FALSE;
}

int get_client_id(char* dest) {
	string mypath;
	FILE * f;
	int ans;
	
	mypath = calloc(1, MAX_PATH_LENGTH);
	
	strcpy(mypath, dest);
	strcat(mypath, "/.cvs");
	
	if ((f = fopen(mypath, "r")) == NULL) {
		printf("Not a repository\n");
		return -1;
	} 
	fscanf(f, "%d", &ans);
	return ans;
}
