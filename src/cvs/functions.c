#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "../../include/structs.h"
#include "../../include/defs.h"

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
    return 0;
}

int add(char* dest, char* file, int client_id) {
	
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
