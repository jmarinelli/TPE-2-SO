#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

#include <signal.h>
#include <errno.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/server/parser.h"


int comChannel;

void server_close(void);

int main() {
	
	instruction_header client_header;
	string instruction_string;
	
	signal(SIGINT, (__sighandler_t)server_close);
	
	parser_init();
	
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
		if(read(comChannel, &client_header, sizeof(instruction_header)) > 0) {
			instruction_string = calloc(1, client_header.instruction_size);

			while(read(comChannel, instruction_string, client_header.instruction_size) == 0)
				sleep(1);
				
			printf("Received instruction from client: %d, instruction: %s\n",
				client_header.client_id, instruction_string);
				
			parse_string(instruction_string);
				
			free(instruction_string);
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
