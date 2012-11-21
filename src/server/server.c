#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

#include <signal.h>
#include <errno.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/server/parser.h"
#include "../../include/filesystem/filesystem.h"


int comChannel;
fstree_t repository_tree;
dlist_t tree_list;



void server_close(void);

int main() {
	
	instruction_header_t client_header = calloc(1, sizeof(instruction_header));
	string instruction_string, client_working_dir, parameter;
	
	signal(SIGINT, (__sighandler_t)server_close);
	
	parser_init();
	repository_tree = filesystem_init();
	repository_tree->tree_id = 0;
	
	tree_list = new_dlist();
		
	if ( mkfifo(SERVER_CHANNEL, 0666) == -1 ) {
		if (errno != EEXIST) {
			/* fatal("Error mkfifo");
			 * 
			 * TODO: Make function fatal in special error file 
			 * 
			 * */
		 }
	}
	
	comChannel = open(SERVER_CHANNEL, O_RDONLY);
	
	while(1) {
		if(read(comChannel, client_header, sizeof(struct instruction_header)) > 0) {
			instruction_string = calloc(1, client_header->instruction_size);
			client_working_dir = calloc(1, client_header->current_path_size + 1);
			parameter = calloc(1, client_header->parameter_size);

			while(read(comChannel, instruction_string, client_header->instruction_size) == 0)
				sleep(1);
			while(read(comChannel, client_working_dir, client_header->current_path_size) == 0)
				sleep(1);
			client_working_dir[client_header->current_path_size] = 0;
			if (client_header->parameter_size > 0) {
				while(read(comChannel, parameter, client_header->parameter_size) == 0)
					sleep(1);
				strcat(instruction_string, " ");
				strcat(instruction_string, parameter);
			}
			
			printf("Received instruction from client: %d, instruction: %s, working dir %s\n",
				client_header->client_id, instruction_string, client_working_dir);
				
			if (parse_string(instruction_string, client_working_dir, client_header->client_id) == -1) {
				printf("%s not a valid instruction\n", instruction_string);
			}
			
		}
		sleep(1);
	}
	return 0;

}


void server_close(){

	printf("Closing server...\n");

	close(comChannel);
	unlink(SERVER_CHANNEL);

	printf("Server closed.\n");

	exit(0);

}
