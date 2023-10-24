Sami Berkan Akkaya ve Osman İlge Ünaldı





To compile the program enter:
	$ make -f makefile.c
command on the terminal.

To run the program enter:
	$ ./server
command on the server terminal

And to run client program enter:
	$ ./client <client_id>
	
to Exit from Client
	write "exit" and enter('\n') while client is active
	
to Send Message
	"message" and enter('\n') while client is active
	"receiver_id" and enter('\n') while client is active
	

command on the client terminal, <client_id> is the name of the client process.

**********

we have developed a program that enables communication between multiple clients using sockets. This communication is orchestrated through a central server, which acts as an intermediary. The server creates and manages individual client handler threads for each connected client, allowing them to exchange data and messages.



