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
#include "transaction.c"

//Global Variables
pthread_t tid;
int serverSocket, connectionSocket;
struct sockaddr_in serverAddress;
int opt = 1;
int addressLen = sizeof(serverAddress);

void sendProblem(int sock)
{
	char * x = "1 Protocol Error";
	write(sock,x,sizeof(x));
}

void* serverThread(void *csocket)
{
	//serverCode
	int total = 0;
	int socket = * (int *)csocket;
	char buffer[BUFFER_SIZE] = {0};
	char toSend[BUFFER_SIZE] = {0};
	int amountRead;

	int type,upc_code,count;
	while(1)
	{
		bzero(buffer, sizeof(buffer)); 
		amountRead = recv(socket,buffer,sizeof(buffer),0);
		if(amountRead<7)
		{
			sendProblem(socket);
			continue;
		}
		sscanf(buffer,"%d %d %d",&type,&upc_code,&count);
		if(type == 1)
		{
			sprintf(toSend,"0 %d",total);
			send(socket,toSend,sizeof(toSend),0);
			break;
		}
		else if(type == 0)
		{
			struct product prd = do_transaction(upc_code);
			if(prd.is_error==1)
			{
				sprintf(toSend,"1 UPC is not found in database");
				send(socket,toSend,sizeof(toSend),0);
				continue;
			}
			total+= prd.price * count;
			sprintf(toSend,"0 %d %s",prd.price,prd.name);
			send(socket,toSend,sizeof(toSend),0);
		}
		else
		{
			sendProblem(socket);
		}
	}
	printf("Closing Socket %d\n",socket);
	close(socket);
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
    	//printf("%s\n","Waiting for new connection" );
    	connectionSocket = accept(serverSocket, (struct sockaddr *)&serverAddress, (socklen_t*)(&addressLen));
    	if(connectionSocket<0)
    	{
    		printf("Connection Error\n");
    		fflush(stdout);
    	}
    	if(pthread_create(&tid, NULL, &serverThread, &connectionSocket))
    	{
    		printf("Error creating thread\n");
    		fflush(stdout);
    	}
    }
}