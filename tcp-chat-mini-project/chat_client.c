#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <pthread.h>

#define BUFFER_SIZE 1024
#define PORT 8000

void* handleServer(void* arg)
{
	int sock_fd = *((int*)arg);
	char buffer[BUFFER_SIZE];

	while(1)
	{
		int bytes_read = 0;
		if( (bytes_read = read(sock_fd, buffer, BUFFER_SIZE - 1)) > 0)
		{
			buffer[bytes_read] = '\0';
			printf("Server: %s\n", buffer);
		}else if(bytes_read < 0)
		{
			perror("server read error");
			close(sock_fd);
		}
	}
	return NULL;
}

int main(int argc, char** argv)
{
	int sock_fd;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in6 server_address;
	pthread_t thread_id;

	if(argc != 2)
	{
		printf("Inncorect usage\n");
		printf("%s <IP_ADDRESS>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if( (sock_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	bzero(&server_address, sizeof(server_address));
	server_address.sin6_family = AF_INET6;
	server_address.sin6_port = htons(PORT);
	
	int err;
	if( (err = inet_pton(AF_INET6, argv[1], &server_address.sin6_addr)) != 1)
	{
		perror("inet_pton error");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}else if(err == 0)
	{
		perror("invalid address family");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}
	
	if(connect(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) 
	{
		perror("connect error");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	if( pthread_create(&thread_id, NULL, handleServer, (void*)&sock_fd) != 0)
	{
		perror("thread error");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}
	
	//sending messages
	while(1)
	{
		fgets(buffer, BUFFER_SIZE, stdin);
		send(sock_fd, buffer, strlen(buffer), 0);
		printf("Client: %s\n", buffer);
	}
}
