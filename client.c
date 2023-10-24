#define _GNU_SOURCE  
#include <linux/sched.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <sched.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <linux/unistd.h>
#include <fcntl.h>
#define PORT 8080
#define STACK_SIZE (1024 * 1024)

char* int2str(int number);
int str2int(char* string);
void message_sender();
void message_receiver();
void transmit_id();
void message_alert(const char* transmitter, const char* message, const char* in_buffer);
int recevier_process();

const char* transmitter;
char* in_buffer;

int client_fd;

int main(int argc, char const* argv[])
{

	transmitter = argv[1];

	in_buffer = (char*)calloc(1, sizeof(char));
	
	int status, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 };
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}
	
	char localIP[15] = "127.0.0.1";
	
	//serv_addr.sin_family = AF_INET;
	//serv_addr.sin_port = htons(PORT);
	
	// Bind to a specific network interface (and optionally a specific local port)
	struct sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = inet_addr(localIP);
	localaddr.sin_port = htons(PORT);  // Any local port will do
	bind(client_fd, (struct sockaddr *)&localaddr, sizeof(localaddr));
	
	// Connect to the remote server
	struct sockaddr_in remoteaddr;
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(localIP);
	remoteaddr.sin_port = htons(PORT);
	connect(client_fd, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr));
	
	
	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	/*if (inet_pton(AF_INET, "193.140.111.16", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((status
		= connect(client_fd, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}*/

	char* stack = malloc(sizeof(char) * STACK_SIZE);

	int pid = clone(recevier_process, stack + STACK_SIZE, CLONE_VM, NULL);

	transmit_id();
	
	while(1)message_sender();

	// closing the connected socket
	close(client_fd);

	return 0;
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

void message_sender() {

	int message_length = 20, receiver_length = 20;
	char *message = (char*)malloc(sizeof(char)*message_length);
	char *receiver = (char*)malloc(sizeof(char)*receiver_length);
	
	char ch;
	int message_size = 0, receiver_size = 0;
	while((ch = getchar()) != '\n') {
		if (message_size >= message_length) {
			in_buffer = realloc(in_buffer, sizeof(char) * (message_length+20));
			message = realloc(message, sizeof(char) * (message_length+=20));
		}
		in_buffer[message_size] = ch;
		message[message_size++] = ch;
		in_buffer[message_size] = '\0';
	}
	message[message_size++] = 0;
	
	if(message[0]=='e' && message[1]=='x' && message[2]=='i' && message[3]=='t'){
		send(client_fd, message, 5, 0);
		exit(0);
	}	
	
	
	while((ch = getchar()) != '\n') {
		if (receiver_size >= receiver_length) {
			in_buffer = realloc(in_buffer, sizeof(char) * (message_length + receiver_length + 20));
			receiver = realloc(receiver, sizeof(char) * (receiver_length+=20));
		}
		in_buffer[message_size + receiver_size] = ch;
		receiver[receiver_size++] = ch;
		in_buffer[message_size + receiver_size] = 0;
		
	}
	receiver[receiver_size++] = 0;
	
	if(receiver[0]=='e' && receiver[1]=='x' && receiver[2]=='i' && receiver[3]=='t'){
		send(client_fd, receiver, 5, 0);
		exit(0);
	}	
	
	int send_buffer_size = message_size + receiver_size + 8;
	
	char* send_buffer = (char*)calloc(send_buffer_size, sizeof(char));
	
	strcat(&send_buffer[0], int2str(message_size));
	strcat(&send_buffer[4], message);
	
	strcat(&send_buffer[4 + message_size], int2str(receiver_size));	
	strcat(&send_buffer[8 + message_size], receiver);
	
		
	//printf("%d\n", send_buffer_size);
	/*for (int k = 0; k < send_buffer_size; k++)
		printf("<%d:%c>,", send_buffer[k], send_buffer[k]);
	printf("\n");*/
	
	int ret = send(client_fd, send_buffer, send_buffer_size, 0);

	if (ret == -1)
		printf("Error!");

	
	free(in_buffer);
	in_buffer = (char*)calloc(1, sizeof(char));

	free(message);
	free(receiver);
	free(send_buffer);
}

void message_receiver() {

	char* message;
	int message_length;
	char buffer[1024] = { 0 };
	while(read(client_fd, buffer, 1024) <= 0);
	
	/*for (int k = 0; k < 30; k++)
		printf("<%d:%c>,", buffer[k], buffer[k]);
	printf("\n");*/
	
	char length_buffer[5] = { 0 };
	strncpy(length_buffer, &buffer[0], 4);
	message_length = str2int(length_buffer);
	message = (char*)malloc(message_length * sizeof(char));
	strcpy(message, &buffer[4]);
	
	strncpy(length_buffer, &buffer[4 + message_length], 4);
	int transmitter_length = str2int(length_buffer);
	char* transmitter_server = (char*)malloc(transmitter_length * sizeof(char));
	strcpy(transmitter_server, &buffer[8 + message_length]);

	message_alert(transmitter_server, message, in_buffer);
	free(message);
}

void transmit_id() {

	int transmitter_size = strlen(transmitter) + 1;
	int send_buffer_size = transmitter_size + 8;
	
	char* send_buffer = (char*)calloc(send_buffer_size, sizeof(char));
	
	strcat(send_buffer, int2str(getpid()));
	strcat(&send_buffer[4], int2str(transmitter_size));
	strcat(&send_buffer[8], transmitter);

	int ret = send(client_fd, send_buffer, send_buffer_size, 0);
}

void message_alert(const char* transmitter, const char* message, const char* in_buffer) {

	printf("\r%s::>%s\n%s", transmitter, message, in_buffer);
}

int recevier_process() {

	while(1)message_receiver();
}












