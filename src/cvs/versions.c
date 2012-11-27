#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/cvs/utils.h"

int get_max_version(string filename, string path) {
	DIR * dir = opendir(path);	
	struct dirent entry;
	struct dirent * result;
	string position;
	int filename_size = strlen(filename);
	int max_version = 0;
	int current_version;
	do {
		readdir_r(dir, &entry, &result);
		if ( strncmp(entry.d_name, ".", 1) && strncmp(entry.d_name, "..", 2)){
			if(!strncmp(entry.d_name, filename, filename_size)) {
				position = (char *)((entry.d_name + filename_size)+1);
				if (*position == '-') {
					sscanf(position, "%d", &current_version);
					if (current_version > max_version)
						max_version = current_version;
				}
			}
		}
	} while ( result != NULL );
	return max_version;
}

void almacenarVersionesViejasEnLaCarpetaOldParaDespuesLeerlasYMostrarselasAlUsuarioParaQueLasUse(string server_path, string old_path, string filename) {	
	int filename_size = strlen(filename);
	char new_file[MAX_PATH_LENGTH];
	string command;
	string file_path = calloc(1, MAX_PATH_LENGTH);
	string mkdir_path = calloc(1, MAX_PATH_LENGTH);
	string position, new_path;
	strcpy(mkdir_path, OLD_REPO_PATH);
	position = strstr(old_path, "old");
	position = strchr(position, '/');
	if (position) {
		strcpy(file_path, position+1);
		while(position = strchr(file_path, '/')) {
			*position = 0;
			if (check_existing_file(mkdir_path, file_path) == NON_EXISTING_FILE) {
				strcat(mkdir_path, "/");
				strcat(mkdir_path, file_path);
				command = build_command(COMMAND_MKDIR, mkdir_path, NULL);
				system(command);
			} else {
				strcat(mkdir_path, "/");
				strcat(mkdir_path, file_path);
			}
			file_path = position + 1;
		}
		if (check_existing_file(mkdir_path, file_path) == NON_EXISTING_FILE) {
			strcat(mkdir_path, "/");
			strcat(mkdir_path, file_path);
			command = build_command(COMMAND_MKDIR, mkdir_path, NULL);
			system(command);
		} else {
			strcat(mkdir_path, "/");
			strcat(mkdir_path, file_path);
		}
	}
	int max_version = get_max_version(filename, old_path);
	sprintf(new_file, "%s-%d", filename, max_version+1);
	new_path = append_to_path(old_path, new_file);
	command = build_command(COMMAND_CP, server_path, new_path);
	system(command); 
}
