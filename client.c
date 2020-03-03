#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "client.h"

int sock;   // Socket, kept as global for sigintHandler

int fin();

void sigintHandler(int sig_num)     // Handles interruption due to user 
{ 
    signal(SIGINT, sigintHandler);  
    fin();                          // Finish the communication
    close(sock);                    // Close the socket
    printf("\nYou stopped me.\n");  // Report the incident
    exit(0);                        // Exit safely
} 

void error(char* msg) {
    printf("ERROR: %s\n", msg);     // Report the error
    exit(1);                        // Exit with error
}

struct response {
    int price;                      // Price of Product
    char name[BUFFERSIZE];          // Name of Product
};                                  // Response struct.

void query(int upc, int num, struct response* resp) {
    char ask[BUFFERSIZE];           // Store query used to ask
    char buf[BUFFERSIZE];           // Store buffer for response
    sprintf(ask, "0 %03d %d", upc, num);        // Format the query string.
    write(sock, ask, sizeof(ask));              // Send the query 

    // Read response 
    
    while (read(sock, buf, sizeof(buf)) == 0); // Read while zero read

    int code;
    int price;
    char name[BUFFERSIZE];

    sscanf(buf, "%d %d %s", &code, &price, name);   // Parse the read response
    
    if (code == 1) {                // Some error has occurred 
        if (name[0] == 'P') { // Protocol error 
            error("Protocol Error!");
        } else {              
            resp->price = -1;   // Object was Invalid
            return;
        }
    }

    resp->price = price;
    strcpy(resp->name, name);

}

int fin() {
    char ask[BUFFERSIZE];
    char buf[BUFFERSIZE];

    sprintf(ask, "1 000 00");
    write(sock, ask, sizeof(ask));

    while (read(sock, buf, sizeof(buf)) == 0);

    int code;
    int total;

    sscanf(buf, "%d %d", &code, &total);

    if (code != 0) {
        error("Protocol error!");
    }

    return total;
}


int valid_upcs[MAX_UPC];
char* names[MAX_UPC];
int prices[MAX_UPC];

void display_index() {
    
    struct response* resp = malloc(sizeof(struct response));

    for (int upc = 0; upc < MAX_UPC; upc++) {
        query(upc, 0, resp);
        if (resp -> price < 0) {
            continue;
        } else {
            printf("(UPC: %03d) Product: %-50s Price: %d\n", upc, resp->name, resp->price);
        }
    }
}

int main(int argc, char* argv[]) {


	signal(SIGINT, sigintHandler); 
    
    sock = 0;

    char* serv_ip = "";
    int serv_port = -1;

    if (argc != 3) {
        error("Usage: <Executable> <Server IP> <Port Number Of Server>");   
    } else {
        serv_ip = argv[1];
        serv_port = atoi(argv[2]);
    }

    for (int j = 0; j < MAX_UPC; j++) {
        valid_upcs[j] = 0;
    }

    struct sockaddr_in serv_addr;


    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Could not create socket.");  
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);

    if (inet_pton(AF_INET, serv_ip, &serv_addr.sin_addr) <= 0) {
        error("Invalid IP.");
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("Connection failed!");
    }

    char send_buffer[BUFFERSIZE];
    char recv_buffer[BUFFERSIZE];
    
    printf("Hello.\n");

    struct response* resp = malloc(sizeof(struct response));

    char buf[BUFFERSIZE];
    
   
    while (1) {
        
        printf("Options: (1) Show Products, (2) Buy Product, (3) Finish Transaction\nChoice (1/2/3)? ");
        int opt;
        scanf("%d", &opt);
        if (opt < 1 || 3 < opt) {
            printf("Incorrect option.\n");
            continue;
        }
        if (opt == 1) {
            
            display_index();

        } else if (opt == 2) {
            
            int upc, num;
            while (1) {
                printf("Which object (UPC)? ");
                scanf("%d", &upc);
                if (upc < 0 || upc >= MAX_UPC) {
                    printf("Invalid UPC.");
                    continue;
                } else break;
            }
            while (1) {
                printf("How many? ");
                scanf("%d", &num);
                if (num < 0) {
                    printf("You cannot buy negatively many products.\n");
                    continue;
                } else break;
            }
           
            query(upc, num, resp);
            
            if (resp->price < 0) {
                printf("Object not found in database!\n");
                continue;
            }

            printf("You bought %d of %s, each priced at %d\n", num, resp->name, resp->price);
        } else {
            int total = fin();
            printf("Your grand total bill: %d.\nBye.\n", total);
            break;
        }
    }

    close(sock);
}
