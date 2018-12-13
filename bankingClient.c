#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<pthread.h> 

struct sockaddr_in serv_addr;
int sockfd;

void * handleInput (void * arg) {
	while (1) {
	int sockfd = *(int*)arg;
 	char sendBuff[1024];
	fgets(sendBuff, 1024, stdin);
	if (strcmp("\n", sendBuff) == 0 || strcmp("", sendBuff) == 0) {
		continue;
	}
	send(sockfd, sendBuff, 1024, 0);
	sendBuff[0] = '\0';
	sleep(2);
	}	
}

int main(int varc, char* argv[])
{

if(varc != 3){
	printf("ERROR: wrong amount of command line arguments");
	return 1;
}
int socket_desc, val;
struct sockaddr_in client_addr;
char buffer[256];
socket_desc = socket(AF_INET, SOCK_STREAM, 0);
struct hostent *server;

int portNumber = atoi (argv[2]);

int port = htons(portNumber);

client_addr.sin_family = AF_INET;

sockfd = socket(AF_INET, SOCK_STREAM, 0);
if(sockfd < 0){
	puts("error opening socket");
	return -1;
}

server = gethostbyname(argv[1]);
if(server == NULL){
	return -1;
}
bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = port;
while(1) {
	puts("Attempting to connect to server...");
	if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) >= 0)
   	{
		printf("Succesfully connected to the server!\n");
		printf("Commands will only be executed every two seconds. If you enter multiple commands in a short time span, they will be executed one at a time every two seconds.\n");
      	break;
   	}
	sleep(3);
}
pthread_t thread_id;
pthread_create( &thread_id , NULL ,  handleInput , &sockfd);
while(1) 
{
	read(sockfd,buffer,255);
	if (strlen(buffer) != 0) {
		printf("Message from server: %s\n",buffer);
		if (strcmp(buffer, "Disconnecting") == 0) {
			break;
		}
		else if(strcmp(buffer, "ServerShutDown") == 0){
			printf("You have disconnected from the sever, goodbye.\n");
			send(sockfd, "Disconnected", 1024, 0);
			break;
		}
		buffer[0] = '\0';
	}
}
	close(socket_desc);
	return 0;	
}
