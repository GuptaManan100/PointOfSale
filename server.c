#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include "server.h"

int main(int argc, char const *argv[]) 
{
	if (argc != 2 || atoi(argv[1])<=0 )
	{
		printf("Usage:- ./server <port number> \n");
		exit(FAILURE);
	}

	int serverSocket, connectionSocket;
	struct sockaddr_in serverAddress;
	int opt = 1;

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) 
    { 
        printf("Socket Creation Failed\n"); 
        exit(FAILURE); 
    } 

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        printf("Socket Already in Use\n");
        exit(EXIT_FAILURE); 
    } 


    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
    serverAddress.sin_port = htons( atoi(argv[1]) ); 

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0) 
    { 
        printf("Server Socket Bind Failed\n"); 
        exit(FAILURE); 
    } 

    if (listen(serverSocket, 5) < 0) 
    { 
        printf("Error in Listen\n"); 
        exit(FAILURE); 
    } 
}