#include "List.h"

List* createList() {

	List* list = (List*)malloc(sizeof(List));
	Client* array = (Client*)malloc(sizeof(Client) * 5);
	list->size = 0;
	list->capacity = 5;
	list->array = array;
	
	return list;
}

void addClient(List* list, char* id, int socket_id, int process_id) {
	
	if (list->size >= list->capacity)
		increaseCapacity(list);
	
	Client client;
	client.id = (char*)malloc(sizeof(char) * (strlen(id) + 1));
	strcpy(client.id, id);
	client.socket_id = socket_id;
	client.process_id = process_id;
	
	list->array[list->size++] = client;
}

void removeClient(List* list, char* id) {
	int flagFound = 0;
	for (int i = 0; i < list->size; i++) {
	
		if(flagFound)
			list->array[i-1] = list->array[i];	
		
		if (strcmp(list->array[i].id, id) == 0)
			flagFound=1;
		
	}
	if(flagFound){
		list->size--;
	}
	else
		printf("Client not found to remove");
}

void increaseCapacity(List* list) {
	
	list->capacity += 5;
	list->array = (Client*)realloc(list->array, sizeof(list->capacity));
}



Client* search(List* list, const char* id) {
	
	for (int i = 0; i < list->size; i++) {
		
		if (strcmp(list->array[i].id, id) == 0)
			return &list->array[i];
	}
	printf("Client not found");
	return NULL;
}

ReturnType getSocket(List* list, const char* id) {
	
	ReturnType ret = { -1, -1 };

	Client* client;
	if ((client = search(list, id)) != NULL) {
		ret.socket = client->socket_id;
		ret.process_id = client->process_id;
	}		
	
	return ret;
}

void printList(List* list) {
	
	for (int i = 0; i < list->size; i++)
		printf("<Id: %s, Socket_id: %d, Process_ID: %d> - ", list->array[i].id, list->array[i].socket_id, list->array[i].process_id);
	printf("\n");	
}





