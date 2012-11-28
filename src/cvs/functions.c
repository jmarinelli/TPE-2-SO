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
	string old_dest;
	strcat(dest,"/cvs");
	
	if ((f = fopen(dest, "r")) != NULL) {
		client_send("The directory already exists", client_id); 
		client_send(END_OF_TRANSMISSION, client_id);
		return 0;
	}
    
    new_dest = remove_last_appended(dest);
    run_command(COMMAND_CP_2, REPOSITORY_PATH, new_dest);
    
    command = append_to_path(dest, "./cvs");
        
    if ((f = fopen(command, "w")) != NULL) {
		fprintf(f, "%d\n", client_id);
		fclose(f);
	}
	
	free(command);
	
	old_dest = append_to_path(dest, "old");

	run_command(COMMAND_RM, old_dest, NULL);
		
	client_send("Checked out", client_id);
	client_send(END_OF_TRANSMISSION, client_id);
	
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






int rename_file(char* dest, char* file, char * new_name, int client_id){
	string local_old_path, server_old_path, local_new_path, server_new_path;
	string position, file_folder;
	string old_repo_path = calloc(1, MAX_PATH_LENGTH);
	string filename, newname, file_path, new_path;
	char old_file[MAX_PATH_LENGTH];
	char old_new_name[MAX_PATH_LENGTH];
	int version;
	struct dirent entry;
	struct dirent * result;
	DIR * old;
	local_old_path = append_to_path(dest, file);
	local_new_path = append_to_path(dest, new_name);
	server_old_path = append_to_path(REPOSITORY_PATH, file);
	server_new_path = append_to_path(REPOSITORY_PATH, new_name);
	if (!get_node_from_path(repository_tree, file)) {
		client_send("File not found in server", client_id);
		client_send(END_OF_TRANSMISSION, client_id);
		return NON_EXISTING_FILE;
	} 
	
	run_command(COMMAND_MV, local_old_path, local_new_path);
	run_command(COMMAND_MV, server_old_path, server_new_path);
	
	file_folder = remove_last_appended(file);
	old_repo_path = append_to_path(OLD_REPO_PATH, file_folder);
	
	if (!(old = opendir(old_repo_path))) {
		client_send("File renamed", client_id);
		client_send(END_OF_TRANSMISSION, client_id);
		return SUCCESS;
	}
	
	filename = get_last_path(file);
	newname = get_last_path(new_name);
	
	do {
		readdir_r(old, &entry, &result);
		if ( strncmp(entry.d_name, ".", 1) && strncmp(entry.d_name, "..", 2)){
			if (!strncmp(filename, entry.d_name, strlen(filename))) {
				position = strchr(entry.d_name, '-');
				if (position) {
					sscanf(position+1, "%d", &version);
					sprintf(old_new_name, "%s-%d", newname, version); 
					file_path = append_to_path(old_repo_path, entry.d_name);
					new_path = append_to_path(old_repo_path, old_new_name);
					run_command(COMMAND_MV, file_path, new_path);
				}
			}
		}
	} while ( result != NULL );

	repository_tree = (fstree_t)new_fstree();
	retrieve_tree(REPOSITORY_PATH, repository_tree->root);
	
	client_send("File renamed.", client_id);
	client_send(END_OF_TRANSMISSION, client_id);
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
		return NON_EXISTING_FILE;
	}
		
	client_send(END_OF_TRANSMISSION, client_id);
	return SUCCESS; 
			
	
}

int add(char* dest, char* file, int client_id) {
	fstree_t client_tree;
	fstree_node_t new_node;
	char * parent_dest, * child_file, * position; 
	char abs_path[MAX_PATH_LENGTH];
	int base_client_id, file_type;
	strcpy(abs_path, dest);
	strcat(abs_path, "/");
	strcat(abs_path, file);
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
			new_node = (fstree_node_t)get_node_from_path(client_tree, file);
			retrieve_tree(abs_path, new_node);
		} else {
			update_child_from_path(client_tree, file, FALSE);
		}
		client_send(END_OF_TRANSMISSION, client_id);
		return SUCCESS;
	} else {
		client_send("File not found in client", client_id);
		client_send(END_OF_TRANSMISSION, client_id);
		return NON_EXISTING_FILE;
	}
}

int commit_recursive(fstree_node_t node, string path, string server_path, string old_path){
	fslist_node_t aux_node;
	fstree_node_t aux_tree_node;
	string new_path;
	string new_server_path;
	string new_old_path;
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
			run_command(COMMAND_CP, path, new_server_path);
			free(new_server_path);
			break;
		case UPDATED:
			almacenarVersionesViejasEnLaCarpetaOldParaDespuesLeerlasYMostrarselasAlUsuarioParaQueLasUse(server_path, remove_last_appended(old_path), node->filename);
			run_command(COMMAND_RM, server_path, NULL);
			new_server_path = remove_last_appended(server_path);
			run_command(COMMAND_CP, path, new_server_path);
			free(new_server_path);
			break;
		case DELETED:
			almacenarVersionesViejasEnLaCarpetaOldParaDespuesLeerlasYMostrarselasAlUsuarioParaQueLasUse(server_path, remove_last_appended(old_path), node->filename);
			run_command(COMMAND_RM, server_path, NULL);
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
	repository_tree = (fstree_t)new_fstree();
	retrieve_tree(REPOSITORY_PATH, repository_tree->root);
	remove_client_tree(base_client_id);
	client_send("Changes commited", client_id);
	client_send(END_OF_TRANSMISSION, client_id);
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
			if (!looping_extra && client_line) {
				sprintf(response, "Extra lines in client file:"); 
				client_send(response, client_id);
				looping_extra = TRUE;
			}
			if (client_line) {
				sprintf(response, "%s", client_line); 
				client_send(response, client_id);
			}
		} else if (!client_line) {
			if (!looping_extra && server_line) {
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
			sprintf(response, "No old versions found in server for file %s", child_file);
			client_send(response, client_id);
			client_send(END_OF_TRANSMISSION, client_id);
			return -1;
		}
		strcat(parent_dest, "/");
		strcat(parent_dest, child_file);
		child_file = position+1;
	}	
	
	version = get_max_version(child_file, parent_dest);
	
	if (!version) {
		sprintf(response, "No old versions found in server for file %s", child_file);
		client_send(response, client_id);
		client_send(END_OF_TRANSMISSION, client_id);
		return -1;
	}
	
	
	sprintf(response, "Versions for file %s found, numbers %d to %d\nType 'cvs backup' to revert changes to older versions.", 
			child_file, 1, version);
	client_send(response, client_id);
	client_send(END_OF_TRANSMISSION, client_id);
}


int backup(char * dest, char * file, int client_id, int version) {
	char backup_file[MAX_PATH_LENGTH], repo_file[MAX_PATH_LENGTH], client_file[MAX_PATH_LENGTH];
	string command;
	FILE * f;
	sprintf(backup_file, "%s/%s-%d", OLD_REPO_PATH, file, version);
	sprintf(repo_file, "%s/%s", REPOSITORY_PATH, file);
	sprintf(client_file, "%s/%s", dest, file);
	if (!(f = fopen(backup_file, "r"))) {
		client_send("Version not found", client_id);
		client_send(END_OF_TRANSMISSION, client_id);
		return NON_EXISTING_FILE;
	}
	run_command(COMMAND_RM, repo_file, NULL);
	run_command(COMMAND_CP, backup_file, repo_file);
	run_command(COMMAND_RM, client_file, NULL);
	run_command(COMMAND_CP, backup_file, client_file);
	client_send("File backed up", client_id);
	client_send(END_OF_TRANSMISSION, client_id);
	return SUCCESS;
}

int checkdir(char * dest, char * dir, int client_id) {
	string repo_dir, old_dir, file_path;
	string position;
	int max_version;
	int version;
	char response[MAX_PATH_LENGTH];
	char filename[MAX_NAME_SIZE];
	DIR * repo, * old;
	struct dirent entry;
	struct dirent * result;
	
	repo_dir = append_to_path(REPOSITORY_PATH, dir);
	old_dir = append_to_path(OLD_REPO_PATH, dir);
	
	if (!(repo = opendir(repo_dir))) {
		client_send("Directory not found in server", client_id);
		client_send(END_OF_TRANSMISSION, client_id);
		return NO_DIRECTORY;
	} else if (!(old = opendir(old_dir))) {
		client_send("No deleted files found", client_id);
		client_send(END_OF_TRANSMISSION, client_id);
		return NO_VERSIONS;
	}
	
	do {
		readdir_r(old, &entry, &result);
		if ( strncmp(entry.d_name, ".", 1) && strncmp(entry.d_name, "..", 2)){
			strcpy(filename, entry.d_name);
			position = strchr(filename, '-');
			if (position) {
				*position = 0;
				sscanf(position+1, "%d", &version);
				file_path = append_to_path(dir, filename);
				if (!get_node_from_path(repository_tree, file_path)) {
					max_version = get_max_version(filename, old_dir);
					if (max_version == version){
						sprintf(response, "Deleted file versions found\nFile: %s\nVersions: %d to %d\nType 'cvs backup %s' to restore.", file_path, 1, max_version, file_path);
						client_send(response, client_id);
					}
				}
				free(file_path);
			}
		}
	} while ( result != NULL );
	client_send(END_OF_TRANSMISSION, client_id);
}
