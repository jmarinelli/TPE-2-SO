#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/cvs/functions.h"


char * command_list[CANT_INSTRUCTIONS];
int (* functions_list[CANT_INSTRUCTIONS]) (char *, char *, int);

void set_lists(void);

int parse_checkout(char * instr, char * cwd, int cid);
int parse_add(char * instr, char * cwd, int cid);
int parse_delete(char * instr, char * cwd, int cid);
int parse_update(char * instr, char * cwd, int cid);
int parse_commit(char * instr, char * cwd, int cid);
int parse_diff(char * instr, char * cwd, int cid);

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

	functions_list[0] = &parse_checkout;
	functions_list[1] = &parse_add;
	functions_list[2] = &parse_delete;
	functions_list[3] = &parse_update;
	functions_list[4] = &parse_commit;
	functions_list[5] = &parse_diff;
}

int parse_string(char * instr, char * cwd, int cid){

	int i, cmd_length, ret;

	if (instr[0] != 0){
		for(i = 0; i < CANT_INSTRUCTIONS; i++){
			cmd_length = strlen(command_list[i]);
			if (!strncmp(instr, command_list[i], cmd_length)){
				return functions_list[i](instr, cwd, cid);
			}
		}
	}
	return -1;
}


int parse_checkout(char * instr, char * cwd, int cid){
	if (strlen(instr) == strlen("checkout")) {
		return checkout(cwd, cid);
	}
	return -1;
}

int parse_add(char * instr, char * cwd, int cid){
	char file[MAX_NAME_SIZE];
	if (sscanf(instr, "add %s", file)){
		if (file != "") {
			return add(cwd, file, cid);
		}
	}
	return -1;
}

int parse_delete(char * instr, char * cwd, int cid){
	char file[MAX_NAME_SIZE];
	if (sscanf(instr, "delete %s", file)){
		if (file != "") {
			return delete(cwd, file, cid);
		}
	}
	return -1;
}

int parse_update(char * instr, char * cwd, int cid){
	if (strlen(instr) == strlen("update")) {
		printf("Update\n");
		//return update(cwd, cid);
	}
	return -1;
}

int parse_commit(char * instr, char * cwd, int cid){
	if (strlen(instr) == strlen("commit")) {
		return commit(cwd, cid);
	}
	return -1;
}

int parse_diff(char * instr, char * cwd, int cid){
	if (strlen(instr) == strlen("diff")) {
		printf("Diff\n");
		//return diff(cwd, cid);
	}
	return -1;
}
