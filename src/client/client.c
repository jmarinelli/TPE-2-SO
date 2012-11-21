#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

#include "../../include/structs.h"
#include "../../include/defs.h"

string pid_to_string(int);

int main(int argc, int ** argv) {
	
	int comChannel;
	
	string instruction, parameter;
	instruction_header client_header;
	
	if (argc < 2) {
		printf("Too few arguments\n");
		/* fatal("Too few arguments") */
		return 0;
	}
	
	if ((comChannel = open(SERVER_CHANNEL, O_WRONLY)) < 0) {}
		// fatal("Server channel closed");
		
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
	
	
	printf("Sending instruction size %d, from client %d\n", client_header.instruction_size, client_header.client_id);
			
	write(comChannel, &client_header, sizeof(struct instruction_header));
	write(comChannel, instruction, client_header.instruction_size);
	write(comChannel, getcwd(0,0), client_header.current_path_size);
	write(comChannel, parameter, client_header.parameter_size);
			
	/* printf("Client pid: %s Read pid %d\n", pid_to_string(getpid()), getpid()); */
	
	
	return 0;
}

string pid_to_string (int pid) {
	
	string ans;
	int aux = pid, cont = 0, i = 0;
	
	while(aux > 0) {
		aux /= 10;
		cont++;
	}
	
	ans = calloc(1, cont + 1);
	aux = pid;
	
	while (aux > 0) {
		ans[cont-(i++)-1] = (aux%10) + '0';
		aux /= 10;
	}
	
	return ans;
	
}
