int checkout(char*,int);
int add(char*,char*,int);
int update(char* dest, char* file, int client_id);
int commit(char * path, int client_id);
int diff(char * path, int client_id);
int delete(char* dest, char* file, int client_id);
int versions(char* dest, char* file, int client_id);
int rename_file(char* dest, char* file, char* new_name, int client_id);
