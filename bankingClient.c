#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct sockaddr_in serv_addr;
int sockfd;


int main(int varc, char* argv[])
{
int socket_desc, val;
struct sockaddr_in client_addr;
char buffer[256];
socket_desc = socket(AF_INET, SOCK_STREAM, 0);
struct hostent *server;


int port = htons(8725);

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
      	break;
   	}
	sleep(3);
}
 char sendBuff[1024];

while(1) 
{
	read(sockfd,buffer,255);
	if (strlen(buffer) != 0) {
		printf("Message from server: %s\n",buffer);
		if (strcmp(buffer, "Disconnecting") == 0) {
			break;
		}
		buffer[0] = '\0';
	}
	fgets(sendBuff, 1024, stdin);
	send(sockfd, sendBuff, 1024, 0);
	sendBuff[0] = '\0';
	
}
	close(socket_desc);
	return 0;	
}
