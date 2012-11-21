#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/shm.h>
#include <sys/stat.h>
#define MAX_LEN 1024



int main (void)
{
   char* p;
   p = (char*)malloc(1000);
   strcat(p,"/home/juanjo/TPE-2-SO/src/cvs2");
   checkout(p);
}

int checkout(char* dest)
{
int status;
char* origin=(char *)malloc(1024);
char* mypath = (char*) malloc(1024);
strcat(mypath,"cp -r ");


if ((status = mkdir(dest, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) == -1) //Aca iria el path
    fprintf(stderr, "El directorio ya existe \n"); 
    
    snprintf(origin, MAX_LEN, "/home/juanjo/lalala"); //Concatenar con path mas cp -r
    strcat(mypath,origin);
    strcat(mypath," ");
    strcat(mypath,dest);
    printf("\n\n\n Se copio aca: %s\n", mypath);
    system(mypath);
}

//Para despues
call_getcwd(void)
{
    char * cwd;
    cwd = getcwd(0, 0);
    if (!cwd) {
	fprintf (stderr, "getcwd failed\n");
    } else {
	printf ("%s\n", cwd);
	free(cwd);
    }
    return;
}
