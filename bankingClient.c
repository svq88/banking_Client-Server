/*#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

int main(int argc, char **argv) {
	
struct sockaddr_in addr;
memset(&addr, 0, sizeof(addr));    /* create & zero struct */
//addr.sin_family = AF_INET;    /* select internet protocol */
//addr.sin_port = argv[2];         /* set the port # */
/*addr.sin_addr.s_addr = argv[1];
int sock = 0, valread; 
char buffer[1024] = {0}; 
char *hello = "Hello from client";

if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
        
   


	while(connect(sock, &addr, sizeof(addr))){
		delay(3000);
	}

send(sock , hello , strlen(hello) , 0 ); 
printf("Hello message sent\n"); 
    valread = read( sock , buffer, 1024); 
    printf("%s\n",buffer ); 
    return 0; 
}*/
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

typedef struct fdNode
{
   int fd;
   struct fdNode *next;
} fdNode;

fdNode * head = NULL;


int main(int varc, char* argv[])
{
int socket_desc, val;
struct sockaddr_in client_addr;
char buffer[256];
socket_desc = socket(AF_INET, SOCK_STREAM, 0);
struct hostent *server;


int port = htons(8723);

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
//char *IPbuffer = inet_ntoa(*((struct in_addr*) 
                        //   hp->h_addr_list[0])); 
//puts(IPbuffer);
//client_addr.sin_addr = *((struct in_addr *)hp->h_addr); 
//client_addr.sin_port = htons(port);

if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR connecting");
      return -1;
   }
puts("connected");
   char sendBuff[1024];

printf("please enter what you wish to do(create<accountName>, serve<accontName> )");
scanf(" %s", sendBuff);
int n = write(sockfd,sendBuff,1024);
      
   if (n < 0) {

      perror("ERROR writing to socket");
      return -1;
   }

char buffer2[1024];
   bzero(buffer2,1024);
   int i;
   n = read(sockfd, buffer2, 1024);
   printf("%s\n",buffer);
   if (n < 0) {
      perror("ERROR reading from socket");
      return -1;
   }

//if(connect(socket_desc, (struct sockaddr*)&client_addr, sizeof(client_addr)) == 0)
//{
//	printf("CONNECT STATE: Connected to Server on port %d\n", port);
//}
//else{
//	printf("Connection to server failed !\n");
//}


while(1) 
{
	puts("here");
	printf("Message to server: ");
	bzero(buffer,256);
	scanf("%s", buffer);
	
	write(socket_desc,buffer,strlen(buffer));
	bzero(buffer,256);
	
	read(socket_desc,buffer,255);
	printf("Message from server: %s\n",buffer);
	
}
	close(socket_desc);
	return 0;	
}