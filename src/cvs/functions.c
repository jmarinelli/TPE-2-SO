#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/cvs/utils.h"
#include "../../include/filesystem/fstree.h"
#include "../../include/filesystem/filesystem.h"

extern fstree_t repository_tree;

int checkout(char* dest, int client_id)
{
	FILE * f;
	string command;
	string new_dest;
	strcat(dest,"/cvs");
	
	if ((f = fopen(dest, "r")) != NULL) {
		printf("The directory already exists \n"); 
		return 0;
	}
    
    new_dest = remove_last_appended(dest);
    command = build_command(COMMAND_CP_2, REPOSITORY_PATH, new_dest);
    system(command);
    
    strcpy(command, dest);
    strcat(command, "/.cvs");
        
    if ((f = fopen(command, "w")) != NULL) {
		fprintf(f, "%d\n", client_id);
		fclose(f);
	}
	
	free(command);
	
    return 0;
}

int update(char* dest, char* file, int client_id){
	fstree_t client_tree;
	fstree_node_t current = client_tree->root;
	char * child_file, * position, *command, *server_folder, *client_folder;
	
	child_file = (char *)calloc(1, MAX_PATH_LENGTH);
	server_folder = (char *)calloc(1, MAX_PATH_LENGTH);
	client_folder = (char *)calloc(1, MAX_PATH_LENGTH);
	command = (char *)calloc(1, MAX_PATH_LENGTH);
	strcpy(child_file, file);
	strcpy(server_folder, REPOSITORY_PATH);
	strcpy(client_folder, dest);
	
	client_tree = repository_tree; 
	
	while (position = strchr(child_file, '/')) {
		*position = 0;
		current =  find_child_by_path(current,child_file);
		if (current == NULL)
			return -1;
		if (check_existing_file(client_folder, child_file) == NON_EXISTING_FILE) {
			strcpy(command,"mkdir ");
			strcpy(command,client_folder); // No sera strcat?
			strcpy(command,child_file);
			system(command);
		}
		strcat(client_folder,"/");
		strcat(client_folder,child_file);
		strcat(server_folder,"/");
		strcat(server_folder,child_file);
		child_file = position+1;
	}
	current =  find_child_by_path(current,child_file);
	
	if (current != NULL){
		strcpy(command,"cp -rf ");
		strcat(command,server_folder);
		strcat(command,"/");
		strcat(command,file);
		strcat(command," ");
		strcpy(command,client_folder);
		strcat(command,file);

		system(command);
	}
	else
		return -1;
		
	return SUCCESS; 
}


int delete(char* dest, char* file, int client_id) {
	
	fstree_t client_tree;
	char * child_file, * position; 
	int base_client_id, file_type;
	child_file = (char *)calloc(1, MAX_PATH_LENGTH);
	strcpy(child_file, file);
	
	base_client_id = get_client_id(dest);
	client_tree = get_client_tree(base_client_id); 
	
	fstree_node_t current = client_tree->root;
	
	while (position = strchr(child_file, '/')) {
		*position = 0;
		current =  find_child_by_path(current,child_file);
		if (current == NULL)
			return -1;
		else 
			current->status = INSIDE_CHANGED;	 
		child_file = position+1;
	}
	current =  find_child_by_path(current,child_file);
	if (current != NULL)
		current->status = DELETED;
	else
		return -1;
	return SUCCESS; 
			
	
}

int add(char* dest, char* file, int client_id) {
	fstree_t client_tree;
	fstree_node_t new_node;
	char * parent_dest, * child_file, * position; 
	char abs_path[MAX_PATH_LENGTH];
	strcpy(abs_path, dest);
	strcat(abs_path, "/");
	strcat(abs_path, file);
	int base_client_id, file_type;
	child_file = (char *)calloc(1, MAX_PATH_LENGTH);
	parent_dest = (char *)calloc(1, MAX_PATH_LENGTH);
	strcpy(parent_dest, dest);
	strcpy(child_file, file);
	while (position = strchr(child_file, '/')) {
		*position = 0;
		if (check_existing_file(parent_dest, child_file) == NON_EXISTING_FILE)
			return -1;
		strcat(parent_dest, "/");
		strcat(parent_dest, child_file);
		child_file = position+1;
	}
	
	file_type = check_existing_file(parent_dest, child_file);
	
	if (file_type != NON_EXISTING_FILE){
		base_client_id = get_client_id(dest);
		if (base_client_id == -1) 
			return;
		client_tree = get_client_tree(base_client_id);
		if (file_type == IS_DIRECTORY) {
			update_child_from_path(client_tree, file, TRUE);
			new_node = find_child_by_path(client_tree->root, child_file);
			retrieve_tree(abs_path, new_node);
		} else {
			update_child_from_path(client_tree, file, FALSE);
		}
		return 0;
	}
	return NON_EXISTING_FILE;
}

int commit_recursive(fstree_node_t node, char * path, char * server_path){
	fslist_node_t aux_node;
	fstree_node_t aux_tree_node;
	string new_path;
	string new_server_path;
	string command;
	switch(node->status) {
		case INSIDE_CHANGED:
			aux_node = node->children->first;
			while (aux_node) {
				aux_tree_node = aux_node->child;
				
				new_path = append_to_path(path, aux_tree_node->filename);
				new_server_path = append_to_path(server_path, aux_tree_node->filename);
				
				commit_recursive(aux_tree_node, new_path, new_server_path);
			
				free(new_path);
				free(new_server_path);
			
				aux_node = aux_node->next;	
			}
			break;
		case ADDED:
			new_server_path = remove_last_appended(server_path);
			command = build_command(COMMAND_CP, path, new_server_path);
			system(command);
			free(new_server_path);
			break;
		case UPDATED:
			command = build_command(COMMAND_RM, server_path, NULL);
			system(command);
			free(command);
			new_server_path = remove_last_appended(server_path);
			command = build_command(COMMAND_CP, path, new_server_path);
			system(command);
			free(command);
			free(new_server_path);
			break;
		case DELETED:
			command = build_command(COMMAND_RM, server_path, NULL);
			system(command);
			free(command);
			break;
		default:
			return;
	}
}

int commit(char * path, int client_id) {
	char destpath[MAX_PATH_LENGTH];
	int base_client_id = get_client_id(path);
	fstree_t client_tree = get_client_tree(base_client_id);
	fstree_node_t current_node = client_tree->root;
	commit_recursive(current_node, path, REPOSITORY_PATH);
}

string read_line(FILE * f) {
	string ans = calloc(1, MAX_PATH_LENGTH);;
	int i = 0;
	char c;
	do {
		c = fgetc(f);
		if (c != EOF && c != '\n')
			ans[i++] = c;
	} while(c != '\n' && c != EOF);
	if (i == 0 && c == EOF)
		return NULL;
	ans[i] = 0;
	return ans;
}

int diff_f(char * local, char * parent_path, char * filename) {
	char * server = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
	string client_line, server_line;
	bool loop_server = TRUE, loop_client = TRUE;
	strcpy(server, parent_path);
	strcat(server, "/");
	strcat(server, filename);
	FILE * local_file = fopen(local, "r");
	FILE * server_file = fopen(server, "r");
	do {
		if (loop_client)
			client_line = read_line(local_file);
		if (loop_server)
			server_line = read_line(server_file);
		if (client_line && server_line) {
			if (strcmp(server_line, client_line))
				printf("Diferencia:\nServer: %s\nClient: %s\n\n", server_line, client_line); 
		} else if (!server_line) {
			if (client_line)
				printf("%s\n", client_line);
		} else if (!client_line) {
			if (server_line)
				printf("%s\n", server_line);
		}
	} while(server_line || client_line);
	fclose(local_file);
	fclose(server_file);
	free(server);
}

int diff_r(char * path, char * server_path, fstree_node_t node, int client_id) {
	if (node == NULL)
		return 0;
	DIR * dir_path = opendir(path);
	fstree_node_t current_node;
	bool is_dir = FALSE;
	struct dirent entry;
	struct dirent * result;
	char * new_path, * last_call, * new_server_path;
	new_path = (char *) calloc(MAX_PATH_LENGTH, sizeof(char));
	last_call = (char *) calloc(MAX_PATH_LENGTH, sizeof(char));
	new_server_path = (char *) calloc(MAX_PATH_LENGTH, sizeof(char));
	strcpy(new_server_path, server_path);
	strcat(new_server_path, "/");
	strcat(new_server_path, node->filename);
	do {
		readdir_r(dir_path, &entry, &result);
		if ( strncmp(entry.d_name, ".", 1) && strncmp(entry.d_name, "..", 2) && strcmp(entry.d_name, last_call) ){
			strcpy(last_call,entry.d_name);
			strcpy( new_path , path );
			strcat( new_path , "/" );
			strcat( new_path , entry.d_name );
			if( entry.d_type == DT_DIR ) {
				is_dir = TRUE;
			}
			current_node = find_child_by_path(node, entry.d_name);
			if (current_node == NULL)
				printf("no encontro: %s\n", entry.d_name);
			else if ( is_dir ) 
				diff_r( new_path , new_server_path, current_node , client_id );
			else 
				diff_f(new_path, new_server_path, current_node->filename);
			is_dir = FALSE;
		}
	} while ( result != NULL );
	free(new_path);
	free(new_server_path);
	free(last_call);
}

int diff(char * path, int client_id) {
	fstree_node_t current_node = repository_tree->root;
	return diff_r(path, "/usr/share", current_node, client_id);
}
