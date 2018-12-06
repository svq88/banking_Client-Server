

#include<stdio.h>
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
    //Get the socket descriptor
    int sock = *(int*)arg;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //end of string marker
		client_message[read_size] = '\0';
		
		//Send the message back to client
        write(sock , client_message , strlen(client_message));
		
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
    
}

void* create(char accountName)
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
     int port =  8723;
    //Create socket

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    if(setsockopt(socket_desc, SOL_SOCKET, 0, &(int) {1}, sizeof(int)) < 0){
    	perror("setsockopt failed");
    }
     
    //Prepare the sockaddr_in structure
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
    puts("bind done");
     
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
        
        if( pthread_create( &thread_id , NULL ,  handle_connection , (void*) &socket_desc) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}


