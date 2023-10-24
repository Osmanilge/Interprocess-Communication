#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	char* id;
	int socket_id;
	int process_id;
} Client;

typedef struct {
	Client* array;
	int size;
	int capacity;
} List;

typedef struct {
	int socket;
	int process_id;
} ReturnType;


List* createList();

void addClient(List* list, char* id, int socket_id, int process_id);

void removeClient(List* list, char* id);

void increaseCapacity(List* list);

Client* search(List* list, const char* id);

ReturnType getSocket(List* list, const char* id);

void printList(List* list);
