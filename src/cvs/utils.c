#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/filesystem/fstree.h"

extern dlist_t tree_list;
extern fstree_t repository_tree;

fstree_t get_client_tree(int client_id){
	dlist_node_t aux_node = tree_list->first;
	fstree_t ans;
	
	while(aux_node) {
		ans = (fstree_t)aux_node->data;
		if (ans->tree_id == client_id)
			return ans;
		aux_node = aux_node->next;
	}
	
	ans = (fstree_t)branch_tree(repository_tree);
	ans->tree_id = client_id;
	dlist_add(ans, sizeof(fstree), tree_list);
	return ans;
}

void remove_client_tree(int client_id) {
	dlist_node_t aux_curr_node = tree_list->first;
	dlist_node_t aux_last_node = tree_list->first;
	fstree_t ans;
	
	while(aux_curr_node) {
		ans = (fstree_t)aux_curr_node->data;
		if (ans->tree_id == client_id) {
			if (aux_curr_node == aux_last_node)
				tree_list->first = aux_curr_node->next;
			else
				aux_last_node->next = aux_curr_node->next;
			return;
		}
		aux_last_node = aux_curr_node;
		aux_curr_node = aux_curr_node->next;
	}
}

int check_existing_file (char* path, char*file) {
	DIR * d = opendir(path);
	struct dirent entry;
	struct dirent * result;
	do {
		readdir_r(d, &entry, &result);
		if ( strncmp(entry.d_name, ".", 1) && strncmp(entry.d_name, "..", 2) ) {
			if (!strcmp(entry.d_name, file)) {
				if (entry.d_type == DT_DIR)
					return IS_DIRECTORY;
				else
					return IS_FILE;
			}
		}
	} while (result != NULL);
	return NON_EXISTING_FILE;
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

string append_to_path(string path, string appendix) {
	string new_path = (string)calloc(1, strlen(path) + strlen(appendix) + 2);
	strcpy(new_path, path);
	if (strcmp(appendix, "")) {
		strcat(new_path,"/");
		strcat(new_path, appendix);
	}
	return new_path;
}

string build_command(string command, string origin, string dest) {
	string com = (string)calloc(1, strlen(command) + strlen(origin) + ((dest) ? strlen(dest) : 0) + SPACE_SIZE);
	strcpy(com, command);
	strcat(com, " ");
	strcat(com,origin);
	if (dest) {
		strcat(com, " ");
		strcat(com, dest);
	}
	return com;
}

void run_command(string command, string origin, string dest) {
	string com;
	com = build_command(command, origin, dest);
	system(com);
	free(com);
}

string remove_last_appended(string path) {
	string new_path = (string)calloc(1, strlen(path) + 1);
	string position = new_path;
	string last_position = new_path;
	strcpy(new_path, path);
	while(position = strchr(position, '/'))
		last_position = position++;
	*last_position = 0;
	return new_path;
}

string get_last_path (string path) {
	string new_path = (string)calloc(1, strlen(path) + 1);
	string position = new_path;
	strcpy(new_path, path);
	while(position = strchr(new_path, '/'))
		new_path = position + 1;
	return new_path;
}

string read_line(FILE * f) {
	string ans = calloc(1, MAX_PATH_LENGTH);;
	int i = 0;
	char c;
	do {
		c = fgetc(f);
		ans[i++] = c;
	} while(c != '\n' && c != EOF);
	if (i == 1 && c == EOF)
		return NULL;
	ans[i-1] = 0;
	return ans;
}
