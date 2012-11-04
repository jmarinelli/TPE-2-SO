#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/structs.h"
#include "../../include/defs.h"


char * command_list[CANT_INSTRUCTIONS];
int (* functions_list[CANT_INSTRUCTIONS]) (char *);

void set_lists(void);

int parse_checkout(char * instr);
int parse_add(char * instr);
int parse_delete(char * instr);
int parse_update(char * instr);
int parse_commit(char * instr);
int parse_diff(char * instr);
int parse_connect(char * instr);

void parser_init() {
	set_lists();
}

void set_lists(){
	command_list[0] = "checkout";
	command_list[1] = "add";
	command_list[2] = "delete";
	command_list[3] = "update";
	command_list[4] = "commit";
	command_list[5] = "diff";
	command_list[6] = "connect";

	functions_list[0] = &parse_checkout;
	functions_list[1] = &parse_add;
	functions_list[2] = &parse_delete;
	functions_list[3] = &parse_update;
	functions_list[4] = &parse_commit;
	functions_list[5] = &parse_diff;
	functions_list[6] = &parse_connect;
}

int parse_string(char * instr){

	int i, cmd_length, ret;

	if (strncmp(instr, "cvs ", CVS_LENGTH))
		return -1;

	if (instr[CVS_LENGTH] != 0){
		for(i = 0; i < CANT_INSTRUCTIONS; i++){
			cmd_length = strlen(command_list[i]) - CVS_LENGTH;
			if (!strncmp(instr + CVS_LENGTH, command_list[i], cmd_length)){
				return functions_list[i](instr);
			}
		}
	}
	return -1;
}


int parse_checkout(char * instr){
	if (strlen(instr) == strlen("cvs checkout")) 
		//return checkout();
	return -1;
}

int parse_add(char * instr){
	char file[MAX_NAME_SIZE];
	if (sscanf(instr, "cvs add %s", file)){
		if (file != "") {
			//return add(file);
		}
	}
	return -1;
}

int parse_delete(char * instr){
	char file[MAX_NAME_SIZE];
	if (sscanf(instr, "cvs delete %s", file)){
		if (file != "") {
			//return delete(file);
		}
	}
	return -1;
}

int parse_update(char * instr){
	if (strlen(instr) == strlen("cvs update")) 
		//return update();
	return -1;
}

int parse_commit(char * instr){
	if (strlen(instr) == strlen("cvs commit")) 
		//return commit();
	return -1;
}

int parse_diff(char * instr){
	if (strlen(instr) == strlen("cvs diff")) 
		//return diff();
	return -1;
}

int parse_connect(char * instr){
	if (strlen(instr) == strlen("cvs connect")) 
		//return connect();
	return -1;
}
