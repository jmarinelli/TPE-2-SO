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
#include "../../include/cvs/versions.h"
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
		client_send("The directory already exists", client_id); 
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
	client_send("Checked out", client_id);
	client_send(END_OF_TRANSMISSION, client_id);
	free(command);
	
    return 0;
}

int update(char* dest, char* file, int client_id){
	fstree_t client_tree;
	client_tree = repository_tree; 
	fstree_node_t current = client_tree->root;
	char * child_file, * position, *command, *server_folder, *client_folder;
	
	child_file = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
	server_folder = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
	client_folder = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
	command = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
	strcpy(child_file, file);
	strcpy(server_folder, REPOSITORY_PATH);
	strcpy(client_folder, dest);
	

	
	while (position = strchr(child_file, '/')) {
		*position = 0;
		current =  find_child_by_path(current,child_file);
		if (current == NULL)
			return -1;
		if (check_existing_file(client_folder, child_file) == NON_EXISTING_FILE) {
			strcpy(command,"mkdir ");
			strcat(command,client_folder);
			strcat(command,"/");
			strcat(command,child_file);
	
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
		strcat(command,child_file);
		strcat(command," ");
		strcat(command,client_folder);
		system(command);
	}
	else {
		client_send("File not found in server", client_id);
		client_send(END_OF_TRANSMISSION, client_id);
		return -1;
	}
	client_send("Updated", client_id);
	client_send(END_OF_TRANSMISSION, client_id);	
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
	current->status = INSIDE_CHANGED;
	
	while (position = strchr(child_file, '/')) {
		*position = 0;
		current =  find_child_by_path(current,child_file);
		if (current == NULL) {
			client_send("File not found", client_id);
			client_send(END_OF_TRANSMISSION, client_id);
			return -1;
		} else { 
			current->status = INSIDE_CHANGED;	 
		}
		child_file = position+1;
	}
	current =  find_child_by_path(current,child_file);
	if (current != NULL) {
		current->status = DELETED;
	} else {
		client_send("File not found", client_id);
		client_send(END_OF_TRANSMISSION, client_id);
		return -1;
	}
		
	client_send(END_OF_TRANSMISSION, client_id);
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
		if (check_existing_file(parent_dest, child_file) == NON_EXISTING_FILE) {
			client_send("File not found in client", client_id);
			client_send(END_OF_TRANSMISSION, client_id);
			return -1;
		}
		strcat(parent_dest, "/");
		strcat(parent_dest, child_file);
		child_file = position+1;
	}
	
	file_type = check_existing_file(parent_dest, child_file);
	
	if (file_type != NON_EXISTING_FILE){
		base_client_id = get_client_id(dest);
		if (base_client_id == -1) { 
			client_send("Not a repository", client_id);
			client_send(END_OF_TRANSMISSION, client_id);
			return -1;
		}
		client_tree = get_client_tree(base_client_id);
		if (file_type == IS_DIRECTORY) {
			update_child_from_path(client_tree, file, TRUE);
			new_node = find_child_by_path(client_tree->root, child_file);
			retrieve_tree(abs_path, new_node);
		} else {
			update_child_from_path(client_tree, file, FALSE);
		}
		client_send(END_OF_TRANSMISSION, client_id);
		return SUCCESS;
	}
	client_send("File not found in client", client_id);
	client_send(END_OF_TRANSMISSION, client_id);
	return NON_EXISTING_FILE;
}

int commit_recursive(fstree_node_t node, string path, string server_path, string old_path){
	fslist_node_t aux_node;
	fstree_node_t aux_tree_node;
	string new_path;
	string new_server_path;
	string new_old_path;
	string command;
	switch(node->status) {
		case INSIDE_CHANGED:
			aux_node = node->children->first;
			while (aux_node) {
				aux_tree_node = aux_node->child;
				
				new_path = append_to_path(path, aux_tree_node->filename);
				new_server_path = append_to_path(server_path, aux_tree_node->filename);
				new_old_path = append_to_path(old_path, aux_tree_node->filename);
				
				commit_recursive(aux_tree_node, new_path, new_server_path, new_old_path);
			
				free(new_path);
				free(new_server_path);
				free(new_old_path);
					
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
			almacenarVersionesViejasEnLaCarpetaOldParaDespuesLeerlasYMostrarselasAlUsuarioParaQueLasUse(server_path, remove_last_appended(old_path), node->filename);
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
			almacenarVersionesViejasEnLaCarpetaOldParaDespuesLeerlasYMostrarselasAlUsuarioParaQueLasUse(server_path, remove_last_appended(old_path), node->filename);
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
	commit_recursive(current_node, path, REPOSITORY_PATH, OLD_REPO_PATH);
	/* Faltaria liberar todo el arbol anterior */
	repository_tree = (fstree_t)new_fstree();
	retrieve_tree(REPOSITORY_PATH, repository_tree->root);
	remove_client_tree(base_client_id);
	client_send("Changes commited", client_id);
	client_send(END_OF_TRANSMISSION, client_id);
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

int diff_f(char * local, char * parent_path, char * filename, int client_id) {
	char * server = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));
	string client_line, server_line;
	int i = 1;
	char response[MAX_RESPONSE_SIZE];
	bool loop_server = TRUE, loop_client = TRUE, looping_extra = FALSE;
	strcpy(server, parent_path);
	strcat(server, "/");
	strcat(server, filename);
	FILE * local_file = fopen(local, "r");
	FILE * server_file = fopen(server, "r");
	sprintf(response, "Differences in file %s\n", local); 
	client_send(response, client_id);
	do {
		if (loop_client)
			client_line = read_line(local_file);
		if (loop_server)
			server_line = read_line(server_file);
		if (client_line && server_line) {
			if (strcmp(server_line, client_line)) {
				sprintf(response, "Difference, line %d:\nServer: %s\nClient: %s\n", 
														i, server_line, client_line); 
				client_send(response, client_id);
			}
		} else if (!server_line) {
			if (!looping_extra) {
				sprintf(response, "Extra lines in client file:"); 
				client_send(response, client_id);
				looping_extra = TRUE;
			}
			if (client_line) {
				sprintf(response, "%s", client_line); 
				client_send(response, client_id);
			}
		} else if (!client_line) {
			if (!looping_extra) {
				sprintf(response, "Extra lines in server file:"); 
				client_send(response, client_id);
				looping_extra = TRUE;
			}
			if (server_line) {
				sprintf(response, "%s", server_line); 
				client_send(response, client_id);
			}
		}
		i++;
	} while(server_line || client_line);
	client_send("\n", client_id);
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
	char response[MAX_RESPONSE_SIZE];
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
		if ( strncmp(entry.d_name, ".", 1) && strncmp(entry.d_name, "..", 2) && strcmp(entry.d_name, last_call) && !strchr(entry.d_name, '~')){
			strcpy(last_call,entry.d_name);
			strcpy( new_path , path );
			strcat( new_path , "/" );
			strcat( new_path , entry.d_name );
			if( entry.d_type == DT_DIR ) {
				is_dir = TRUE;
			}
			current_node = find_child_by_path(node, entry.d_name);
			if (current_node == NULL) {
				sprintf(response, "File or Folder %s/%s not found in repository\n"
							, path, entry.d_name);
				client_send(response, client_id);
			}
			else if ( is_dir ) { 
				diff_r( new_path, new_server_path, current_node, client_id );
			} else {
				diff_f(new_path, new_server_path, current_node->filename, client_id);
			}
			is_dir = FALSE;
		}
	} while ( result != NULL );
	if (!strcmp(node->filename, CVS_ROOT_NAME))
		client_send(END_OF_TRANSMISSION, client_id);
	free(new_path);
	free(new_server_path);
	free(last_call);
}

int diff(char * path, int client_id) {
	fstree_node_t current_node = repository_tree->root;
	return diff_r(path, "/usr/share", current_node, client_id);
}


int versions(char* dest, char* file, int client_id) {
	fstree_node_t new_node;
	char response[MAX_RESPONSE_SIZE];
	int version;
	char * parent_dest, * child_file, * position; 
	child_file = (char *)calloc(1, MAX_PATH_LENGTH);
	parent_dest = (char *)calloc(1, MAX_PATH_LENGTH);
	strcpy(parent_dest, OLD_REPO_PATH);
	strcpy(child_file, file);
	while (position = strchr(child_file, '/')) {
		*position = 0;
		if (check_existing_file(parent_dest, child_file) == NON_EXISTING_FILE) {
			client_send("No old versions found server", client_id);
			client_send(END_OF_TRANSMISSION, client_id);
			return -1;
		}
		strcat(parent_dest, "/");
		strcat(parent_dest, child_file);
		child_file = position+1;
	}	
	
	version = get_max_version(child_file, parent_dest);
	sprintf(response, "Versions for file %s found, numbers %d to %d\nType 'cvs backup' to revert changes to older versions.", 
			child_file, 1, version);
	client_send(response, client_id);
	client_send(END_OF_TRANSMISSION, client_id);
}
