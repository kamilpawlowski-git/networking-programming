#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <pthread.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define PORT 8000
#define LISTENQ 2

void handleSigpipe(int sig)
{
	printf("SIGPIPE signal recived\n");
	printf("User disconnected\n");
	exit(1);
}

void* handleClient(void* arg)
{
	int new_socket = *((int*)arg);
	char buffer[BUFFER_SIZE];

	while(1)
	{	
		int bytes_read = 0;
		if( (bytes_read = read(new_socket, buffer, BUFFER_SIZE - 1)) > 0 )
		{
			buffer[bytes_read] = '\0';
			printf("Client: %s\n", buffer);
		}else if(bytes_read < 0)
		{
			perror("client read error");
			close(new_socket);		
		}
	}
	return NULL;
}

int main()
{
	int server_fd, new_socket;
	struct sockaddr_in6 server_address;
	char buffer[BUFFER_SIZE];
	pthread_t client_thread;

	if( (server_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0 )
	{
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	bzero(&server_address, sizeof(server_address));
	server_address.sin6_family = AF_INET6;
	server_address.sin6_addr = in6addr_any;
	server_address.sin6_port = htons(PORT);

	if(bind(server_fd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
	{
		perror("bind error");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	if(listen(server_fd, LISTENQ) < 0) //maly blad do poprawy
	{
		perror("listen error");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	
	int addrlen = sizeof(server_address);
	if( (new_socket = accept(server_fd, (struct sockaddr*) &server_address, &addrlen)) < 0)
	{
		perror("accept error");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	
	if(pthread_create(&client_thread, NULL, handleClient, (void*)&new_socket) != 0)
	{
		perror("thread error");
		close(new_socket);
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	
	signal(SIGPIPE, handleSigpipe);

	//sending messages and printing server (we) message locally
	while(1)
	{
		if(fgets(buffer, BUFFER_SIZE, stdin) != NULL);
		{
			printf("Server: %s\n", buffer);
			send(new_socket, buffer, strlen(buffer), 0);
		}

	}
}
