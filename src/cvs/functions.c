#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/cvs/utils.h"
#include "../../include/filesystem/fstree.h"

int checkout(char* dest, int client_id)
{
	int status;
	FILE * f;
	char* origin=(char *)malloc(MAX_PATH_LENGTH);
	char* mypath = (char*) malloc(MAX_PATH_LENGTH);
	strcat(mypath,"cp -r ");
	strcat(dest,"/cvs");
	
	if ((f = fopen(dest, "r")) != NULL) {
		printf("The directory already exists \n"); 
		return 0;
	}
    
    snprintf(origin, strlen(REPOSITORY_PATH)+1, REPOSITORY_PATH);
    strcat(mypath, origin);
    strcat(mypath," ");
    strcat(mypath,dest);
    system(mypath);
    
    strcpy(mypath, dest);
    strcat(mypath, "/.cvs");
        
    if ((f = fopen(mypath, "w")) != NULL) {
		fprintf(f, "%d\n", client_id);
		fclose(f);
	}
	
    return 0;
}

int add(char* dest, char* file, int client_id) {
	fstree_t client_tree;
	fstree_node_t new_node;
	char * parent_dest, * child_file, * position; 
	char abs_path[MAX_PATH_LENGTH];
	strcpy(abs_path, dest);
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

//Para despues
call_getcwd(void)
{
    char * cwd;
    cwd = (string)getcwd(0, 0);
    if (!cwd) {
		fprintf (stderr, "getcwd failed\n");
    } else {
		printf ("%s\n", cwd);
		free(cwd);
    }
    return;
}
