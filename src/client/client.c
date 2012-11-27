#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/server/server.h"

string get_string_from_pid(int);

int main(int argc, int ** argv) {
	
	int comChannel, respChannel;
	int response_size;
	
	bool loop = TRUE;
	
	string instruction, parameter, parameter2, response, string_pid;
	instruction_header client_header;
	
	if (argc < 2) {
		printf("Too few arguments\n");
		return 0;
	}
	
	if ((comChannel = open(SERVER_CHANNEL, O_WRONLY)) < 0) {
		printf("Server channel is closed\n");
		return NON_EXISTING_FILE;
	}
		
	instruction = (string)argv[1];
	client_header.client_id = getpid();
	client_header.instruction_size = strlen(instruction);
	client_header.current_path_size = strlen((string)getcwd(0,0));
		
	if (argc < 3) {
		client_header.parameter_size = 0;
		parameter = "";
	} else {
		parameter = (string)argv[2];
		client_header.parameter_size = strlen(parameter);
	}
	
	if (argc < 4) {
		client_header.parameter_size2 = 0;
		parameter2 = "";
	} else {
		parameter2 = (string)argv[3];
		client_header.parameter_size2 = strlen(parameter2);
	}
	
	
	printf("Sending instruction size %d, from client %d\n", client_header.instruction_size, client_header.client_id);
			
	write(comChannel, &client_header, sizeof(struct instruction_header));
	write(comChannel, instruction, client_header.instruction_size);
	write(comChannel, getcwd(0,0), client_header.current_path_size);
	write(comChannel, parameter, client_header.parameter_size);
	write(comChannel, parameter2, client_header.parameter_size2);
	
	close(comChannel);
	
	string_pid = get_string_from_pid(getpid());
	
	if ( mkfifo(string_pid, 0666) == -1 ) {
		if (errno != EEXIST) {
			printf("Couldn't make client fifo\n");
			return NO_MEMORY;
		 }
	}
		
	respChannel = open(string_pid, O_RDONLY);
	
	do {
		if (read(respChannel, &response_size, sizeof(int)) > 0) {
			response = calloc(1, response_size);
			while(read(respChannel, response, response_size) <= 0)
				sleep(1);
			loop = strcmp(END_OF_TRANSMISSION, response);
			if (loop)
				printf("%s\n", response);
			free(response);
		} else {
			sleep(1);
		}
	} while (loop);
	
	close(respChannel);
	unlink(string_pid);
	
	return 0;
}


string get_string_from_pid (int pid)
{
	string file = calloc(1, MAX_PATH_LENGTH);
	sprintf(file, "/tmp/%d", pid);
	return file;
}
