program: server.c client.c List.h List.c
	gcc server.c List.c -o server
	gcc client.c -o client

