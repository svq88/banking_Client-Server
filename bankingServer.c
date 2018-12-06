

#include<stdio.h>
#include<errno.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> 
//S#include "bankingServer.h"

//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



void* handle_connection(void *arg)
{

    int sock = *(int*)arg;
    int read_size;
    char *message , client_message[1024];
    char buffer[] = "Please enter what you wish to do(create <accountName>, serve <accontName> )\0";
	send(sock , buffer , 255, 0);

    while(1) {
		read(sock,client_message,1024);
		if (strlen(client_message) != 0) {
			printf("Message from client: %s\n",client_message);
			if (strcmp("query\n", client_message) == 0) {
				send(sock, "Querying", 255, 0);
				continue;
			} else if (strcmp("end\n", client_message) == 0) {
				send(sock, "Ending", 255, 0);
				continue;
			} else if (strcmp("quit\n", client_message) == 0) {
				send(sock, "quitting", 255, 0);
				continue;
			}
			char * command = strtok(client_message, " ");
			if (strcmp("create", command) == 0) {
				command = strtok(NULL, " ");
				if (strcmp("create", command) == 0 || strcmp("", command) == 0) {
					char error[] = "Please enter a username - create <username>.";
					send(sock, error, 255, 0);
					continue;
				}
				int length = strlen(command);
				command[length - 1] = '\0';
				char results[255];
				sprintf(results, "Succesfully created \"%s\"!", command);
				send(sock, results, 255, 0);
			} else if (strcmp("serve", command) == 0) {
				command = strtok(NULL, " ");
				if (strcmp("create", command) == 0 || strcmp("", command) == 0) {
					char error[] = "Please enter a username - create <username>.";
					send(sock, error, 255, 0);
					continue;
				}
				int length = strlen(command);
				command[length - 1] = '\0';
				char results[255];
				sprintf(results, "Succesfully accessed \"%s\"!", command);
				send(sock, results, 255, 0);

			} else if (strcmp("deposit", command) == 0) {
				command = strtok(NULL, " ");
				char * endptr;
				errno = 0;
				double amt = strtod(command, &endptr);
				printf("errno %d\n", errno);
				if (amt == 0 && (errno != 0 || command == endptr)) {
					char doubleError[] = "Please input a valid double -- deposit <double Amount>. E.g. withdraw 100.05\n";
					send(sock , doubleError , 255, 0);
					continue;
				}
				char buff[255];
				sprintf(buff, "Succesfully deposited %f!\n", amt);
				send(sock , buff , 255, 0);
				
			} else if (strcmp("withdraw", command) == 0) {
				command = strtok(NULL, " ");
				char * endptr;
				errno = 0;
				double amt = strtod(command, &endptr);
				printf("errno %d\n", errno);
				if (amt == 0 && (errno != 0 || command == endptr)) {
					char doubleError[] = "Please input a valid double -- withdraw <double Amount>. E.g. withdraw 100.05\n";
					send(sock , doubleError , 255, 0);
					continue;
				}
				char buff[255];
				sprintf(buff, "Succesfully withdrew %f!\n", amt);
				send(sock , buff , 255, 0);
			
			} else {
				char error[] = "That is not a valid command. Use one of the following:\ncreate <accountName>\nserver <accountName>\ndeposit <double Amount>\nwithdraw <double Amount>\nquery\nend\nquit";
				send(sock, error, 255, 0);
			}
			client_message[0] = '\0';
		}
	}
    
         
    return 0;
    
}

void* create(char * accountName)
{

}

void* serve(char accountName)
{

}

void* deposit(double amount)
{

}

void* widthdrawl(double amount)
{

}

void* query()
{

}

void* end()
{

}

void* quit()
{

}

int main(void)
{
    /* do the necessary setup, i.e. bind() and listen()... */
    
int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     int sockfd, newsocket, length;
     int port =  8725;
    //Create socket

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    if(setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0){
    	perror("setsockopt failed");
    }
     
    bzero((char*) &server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port =  htons(port);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *) &server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
     
    //Listen
    listen(socket_desc , 5);

     length = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    
	pthread_t thread_id;
	
    while( 1 )
    {
    	int * socketPtr = malloc(sizeof(int));
        //puts("Connection accepted");
        newsocket = accept(socket_desc, (struct sockaddr *) &client, (socklen_t *) &c);
        if( pthread_create( &thread_id , NULL ,  handle_connection , &newsocket) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        pthread_join( thread_id , NULL);
		
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}


