#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

#include "../../include/structs.h"
#include "../../include/defs.h"

string pid_to_string(int);

int main(int argc, int ** argv) {
	
	int comChannel;
	
	string instruction;
	instruction_header client_header;
	
	if (argc < 2) {
		/* fatal("Too few arguments") */
		return 0;
	}
	
	if ((comChannel = open(SERVER_CHANNEL, O_WRONLY)) < 0)
		// fatal("Server channel closed");
	
	instruction = (string)argv[1];
	
	client_header.client_id = getpid();
	client_header.instruction_size = strlen(instruction);
			
	write(comChannel, client_header, sizeof(instruction_header));
	write(comChannel, instruction, client_header.instruction_size);
			
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
