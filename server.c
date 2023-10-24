#define _GNU_SOURCE  
#include <linux/sched.h>

#include <netinet/in.h>
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/unistd.h>
#include <errno.h>

#include "List.h"

#define PORT 8080
#define STACK_SIZE (1024 * 1024)

typedef struct {
	int socket;
	List* list;
}Arg;

int str2int(char* string);
char* int2str(int number);
int handle_client(void* socket);

List* list;

int main(int argc, char const* argv[])
{

	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char* hello = "Hello from server";
	
	//Listeyi olduşturdum. Ayrıntı için List.c'ye bak.
	list = createList();
  
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
  
	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET,
				   SO_REUSEADDR | SO_REUSEPORT, &opt,
				   sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
  
	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,
			 sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	while (1) {
	
		if (listen(server_fd, 3) < 0) {
			perror("listen");
			exit(EXIT_FAILURE);
		}
		if ((new_socket
			 = accept(server_fd, (struct sockaddr*)&address,
					  (socklen_t*)&addrlen))
			< 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		
		// Buraya kadar olan kısımlar socket bağlantısı ile alakalı.
		// accept metodu çağrıldığında client'a bağlanıldı.
		
		Arg arg = { new_socket, list };

		char* stack = malloc(STACK_SIZE * sizeof(char));
		
		// clone metodu ile yeni thread oluşturuldu.
		int pid = clone(handle_client, stack + STACK_SIZE, CLONE_VM | CLONE_FILES, (void*)(&arg));
	
	}
  
	// closing the connected socket
	close(new_socket);
	// closing the listening socket
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}

int str2int(char* string) {

	
	int digit_value = 1;
	int result = 0;
	int len = strlen(string);
	for (int i = 0; i < len; i++) {
		result += (string[len - 1 - i] - '0') * digit_value;
		digit_value *= 10;
	}
	return result;
}

char* int2str(int number) {
	
	char* output_string = (char*)malloc(5 * sizeof(char));
	for (int i = 0; i < 4; i++)
		output_string[i] = '0';
	for (int i = 0; number > 0; i++) {
		output_string[3-i] = number % 10 + '0';
		number /= 10;
	}
	output_string[4] = 0;
	return output_string;
}

int handle_client(void* arg) {

	
	int new_socket = ((Arg*)arg)->socket;
	List* list = (((Arg*)arg)->list);

	char buffer[1024] = { 0 };
	
	read(new_socket, buffer, 1024);
	
	for (int i = 0; i < 15; i++)
		printf("<%d:%c>,", buffer[i], buffer[i]);
	printf("\n");
	
	
	char length_buffer[5] = { 0 };
	
	strncpy(length_buffer, &buffer[0], 4);
	int process_id = str2int(length_buffer);
	strncpy(length_buffer, &buffer[4], 4);
	int transmitter_length = str2int(length_buffer);
	char *transmitter = (char*)malloc(transmitter_length * sizeof(char));
	strcpy(transmitter, &buffer[8]);

	printf("Connected to %s, socket: %d, PID: %d\n", transmitter, new_socket, process_id);
	
	addClient(list, transmitter, new_socket, process_id);

	printList(list);
	while(1){
		while(read(new_socket, buffer, 1024)<=0);
		
		if(buffer[0]=='e' && buffer[1]=='x' && buffer[2]=='i' && buffer[3]=='t'){
			buffer[0]==0;
			removeClient(list, transmitter);
			printList(list);
			break;
		}
		
		for (int i = 0; i < 30; i++)
			printf("<%d:%c>,", buffer[i], buffer[i]);
		printf("\n");
	
		strncpy(length_buffer, &buffer[0], 4);
		int message_length = str2int(length_buffer);
		char *message = (char*)malloc(message_length * sizeof(char));
		strcpy(message, &buffer[4]);
	
		strncpy(length_buffer, &buffer[4 + message_length], 4);
		int receiver_length = str2int(length_buffer);
		char *receiver = (char*)malloc(receiver_length * sizeof(char));
		strcpy(receiver, &buffer[8 + message_length]);
	
		printf("\n%s::%s->%s\n", message, transmitter, receiver);
	
		ReturnType receiver_ret;
		if ((receiver_ret = getSocket(list, receiver)).socket == -1) {
		
			char* error = (char*)calloc(strlen(receiver) + 20, sizeof(char));
			sprintf(error, "Client %s NOT FOUND", receiver);
			printf("\n%s\n", error);
			//send(new_socket, error, strlen(error), 0);
			free(error);
			continue;
		}
		
	
		//transmitterlenth nedense 1 eksik
		int send_buffer_length = 8 + strlen(message) + strlen(transmitter)+1;
	
		char* send_buffer = (char*)calloc(8 + strlen(message) + strlen(transmitter)+1, sizeof(char));
		strcat(&send_buffer[0], int2str(message_length));
		strcat(&send_buffer[4], message);
	
		strcat(&send_buffer[4 + message_length], int2str(strlen(transmitter)+1));	
		strcat(&send_buffer[8 + message_length], transmitter);
		
		if (send(receiver_ret.socket, send_buffer, send_buffer_length, 0) < 0) {
		
		printf("Value of errno: %d\n ", errno);
		    printf("The error message is : %s\n", 
				         strerror(errno));
		}
	}
	
	return 1;
}
