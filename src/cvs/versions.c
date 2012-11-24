#include <stdio.h>
#include <string.h>

#include <dirent.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/cvs/utils.h"

void almacenarVersionesViejasEnLaCarpetaOldParaDespuesLeerlasYMostrarselasAlUsuarioParaQueLasUse(string server_path, string old_path, string filename) {
	DIR * dir = opendir(old_path);	
	int filename_size = strlen(filename);
	int max_version = 0;
	int current_version;
	string position;
	string command;
	string new_path;
	char new_file[MAX_PATH_LENGTH];
	struct dirent entry;
	struct dirent * result;
	do {
		readdir_r(dir, &entry, &result);
		if ( strncmp(entry.d_name, ".", 1) && strncmp(entry.d_name, "..", 2)){
			if(!strncmp(entry.d_name, filename, filename_size)) {
				position = (char *)(entry.d_name + filename_size);
				sscanf(position, "%d", &current_version);
				if (current_version > max_version)
					max_version = current_version;
			}
		}
	} while ( result != NULL );
	sprintf(new_file, "%s-%d", filename, max_version+1);
	new_path = append_to_path(old_path, new_file);
	command = build_command(COMMAND_CP, server_path, new_path);
	system(command); 
}
