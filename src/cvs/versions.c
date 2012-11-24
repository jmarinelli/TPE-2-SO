#include <stdio.h>
#include <string.h>

#include <dirent.h>

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
				sscanf(position, "%d", &current_version);
				if (current_version > max_version)
					max_version = current_version;
			}
		}
	} while ( result != NULL );
	return max_version;
}

void almacenarVersionesViejasEnLaCarpetaOldParaDespuesLeerlasYMostrarselasAlUsuarioParaQueLasUse(string server_path, string old_path, string filename) {	
	int filename_size = strlen(filename);
	char new_file[MAX_PATH_LENGTH];
	string command;
	string new_path;
	int max_version = get_max_version(filename, old_path);
	sprintf(new_file, "%s-%d", filename, max_version+1);
	new_path = append_to_path(old_path, new_file);
	command = build_command(COMMAND_CP, server_path, new_path);
	system(command); 
}
