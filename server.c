#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h> 
#include <pthread.h>
#include "server.h"
#include "lock.h"

//Global Variables
pthread_t tid;
int serverSocket, connectionSocket;
struct sockaddr_in serverAddress;
int opt = 1;
int addressLen = sizeof(serverAddress);

void* serverThread(void *csocket)
{
	//serverCode
	int total = 0;
	int socket = * (int *)csocket;

	return NULL;
}

void sigintHandler(int sig_num) 
{ 
    signal(SIGINT, sigintHandler); 
    close(serverSocket);
    printf("\nExited Gracefully\n");
    exit(0);
} 

int main(int argc, char const *argv[]) 
{
	signal(SIGINT, sigintHandler); 

	if (argc != 2 || atoi(argv[1])<=0 )
	{
		printf("Usage:- ./server <port number> \n");
		exit(FAILURE);
	}

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

    if (listen(serverSocket, BACKLOG) < 0) 
    { 
        printf("Error in Listen\n"); 
        exit(FAILURE); 
    } 

    while(1)
    {
    	connectionSocket = accept(serverSocket, (struct sockaddr *)&serverAddress, (socklen_t*)(&addressLen));
    	if(connectionSocket<0)
    	{
    		printf("Connection Error\n");
    	}
    	if(pthread_create(&tid, NULL, &serverThread, &connectionSocket))
    	{
    		printf("Error creating thread\n");
    	}
    }
}