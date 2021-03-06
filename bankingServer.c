#include<stdio.h>
#include<errno.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> 
#include <signal.h> 
#include<semaphore.h>
#include <sys/time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct account {
	int inUse;
	double balance;
	char accountName[255];
} account;

int socketIds[10000];
pthread_t threadIds[10000];
int clientCount = 0;

account accounts[10000];

int numberOfElements = 0;

int threadCount = 0;

static sigset_t   mask;

sem_t semaphore;

int serve(char * accountName)
{
	int i = 0;
	pthread_mutex_lock(&mutex);
	while(i < numberOfElements){
		if(strcmp(accounts[i].accountName, accountName) == 0){
			if (accounts[i].inUse == 1) {
				pthread_mutex_unlock(&mutex);
				return -2;
			}
			accounts[i].inUse = 1;
			pthread_mutex_unlock(&mutex);
			return i;
		}
		i++;
	}
	pthread_mutex_unlock(&mutex);
	return -1;//account does not exist
}

double deposit(double amount, int accountNum)
{
	pthread_mutex_lock(&mutex);
	accounts[accountNum].balance += amount;
	double balance = accounts[accountNum].balance;
	pthread_mutex_unlock(&mutex);
	return balance;
}

double withdraw(double amount, int accountNum)
{
	pthread_mutex_lock(&mutex);
	if(accounts[accountNum].balance < amount){
		pthread_mutex_unlock(&mutex);
		return -1;
	}
	else	{
		accounts[accountNum].balance -= amount;
		double balance = accounts[accountNum].balance;
		pthread_mutex_unlock(&mutex);
		return balance;
	}
}

double query(int accountNum)
{
	pthread_mutex_lock(&mutex);
	double amt  = accounts[accountNum].balance;
	pthread_mutex_unlock(&mutex);
	return amt;	 
}

int createAccount(char * accountName) {
	if (numberOfElements == 10000) {
		return -2;//max amt of accounts
	}

	pthread_mutex_lock(&mutex);
	int i;
	for (i = 0; i < numberOfElements; i++) {
		if (strcmp(accountName, accounts[i].accountName) == 0) {
			pthread_mutex_unlock(&mutex);
			return -1; //acc exists.
		}
	}
	account account;
	strcpy(account.accountName, accountName);
	accounts[numberOfElements] = account;
	pthread_mutex_unlock(&mutex);
	return numberOfElements++;
}


int end(int accountNum)
{
	if (accountNum == -1) {
		return -1;
	}
	pthread_mutex_lock(&mutex);
	accounts[accountNum].inUse = 0;
	pthread_mutex_unlock(&mutex);
	return -1;
}


void* handle_connection(void *arg)
{

    int sock = *(int*)arg;
    int read_size;
    char *message , client_message[1024];
    char buffer[] = "Please enter what you wish to do(create <accountName>, serve <accontName> )\0";
	send(sock , buffer , 255, 0);
	int servedAccount = -1;
	//read to see if disconnect

    while(1) {
		read(sock,client_message,1024);
		if (strlen(client_message) != 0) {
			printf("Message from client: %s\n",client_message);
			sem_wait(&semaphore);
			if (strcmp("query\n", client_message) == 0) {
				if (servedAccount == -1) {
					send(sock, "You need to access an account using the serve command first.\n", 255, 0);
				sem_post(&semaphore);
					continue;
				}
				double amt = query(servedAccount);
				char buff[255];
				sprintf(buff, "Current Balance: %f\n", amt);
				send(sock, buff, 255, 0);
				sem_post(&semaphore);
				continue;
			} else if (strcmp("end\n", client_message) == 0) {
				servedAccount = end(servedAccount);
				send(sock, "Ending service with current account", 255, 0);
				sem_post(&semaphore);
				continue;
			} else if (strcmp("quit\n", client_message) == 0) {
				end(servedAccount);
				pthread_mutex_lock(&mutex);
				send(sock, "Disconnecting", 255, 0);
				shutdown(sock, SHUT_RDWR);
				close(sock);
				printf("Disconnected from a client.\n");
				threadCount--;
				pthread_mutex_unlock(&mutex);
				sem_post(&semaphore);
				return;
			}
			else if(strcmp("Disconnected", client_message) == 0){
				pthread_mutex_lock(&mutex);
				shutdown(sock, SHUT_RDWR);
				close(sock);
				threadCount--;
				pthread_mutex_unlock(&mutex);
				return;
			}
			int length = strlen(client_message);
			client_message[length - 1] = '\0';
			char * user = strdup(client_message);
			char * command = strtok(client_message, " ");
			if (strcmp("create", command) == 0) {
				if (servedAccount != -1) {
					send(sock, "You cannot create an account while accessing one.\n", 255, 0);
				sem_post(&semaphore);
					continue;
				}
				command = strtok(NULL, " ");
				if (strcmp("create", command) == 0 || strcmp("", command) == 0) {
					char error[] = "Please enter a username - create <username>.";
					send(sock, error, 255, 0);
				sem_post(&semaphore);
					continue;
				}
				user += 7;
				int check = createAccount(user);
				char results[255];
				if (check == -1) {
					sprintf(results, "Failed to create \"%s\", it already exists!", user);
				} else if (check == -2) {
					sprintf(results, "You've reached the max amount of accounts: 10,000");
				} else {
					sprintf(results, "Succesfully created \"%s\"!", user);
				}
				send(sock, results, 255, 0);
			} else if (strcmp("serve", command) == 0) {
				if (servedAccount != -1) {
					send(sock, "You are already accessing an account.\n", 255, 0);
				sem_post(&semaphore);
					continue;
				}
				command = strtok(NULL, " ");
				if (strcmp("create", command) == 0 || strcmp("", command) == 0) {
					char error[] = "Please enter a username - create <username>.";
					send(sock, error, 255, 0);
					sem_post(&semaphore);
					continue;
				}
				user += 6;
				char results[255];
				servedAccount = serve(user);
				if (servedAccount == -1) {
					sprintf(results, "Failed to access \"%s\", account does not exist!\n", user);
				} else if (servedAccount == -2) { 
					servedAccount = -1;
					sprintf(results, "Failed to access \"%s\", the account is already being accessed.!\n", user);
				}else {
					sprintf(results, "Succesfully accessed \"%s\"!\n", user);
				}
				send(sock, results, 255, 0);

			} else if (strcmp("deposit", command) == 0) {
				if (servedAccount == -1) {
					send(sock, "You need to access an account using the serve command first.\n", 255, 0);
					sem_post(&semaphore);
					continue;
				}
				command = strtok(NULL, " ");
				char * endptr;
				errno = 0;
				double amt = strtod(command, &endptr);
				if ((amt == 0 && (errno != 0 || command == endptr)) || amt < 0) {
					char doubleError[] = "Please input a valid double -- deposit <double Amount>. E.g. withdraw 100.05\n";
					send(sock , doubleError , 255, 0);
					sem_post(&semaphore);
					continue;
				}
				char buff[255];
				double total = deposit(amt, servedAccount);
				sprintf(buff, "Succesfully deposited %f, Current Total: %f!\n", amt, total);
				send(sock , buff , 255, 0);
				
			} else if (strcmp("withdraw", command) == 0) {
				if (servedAccount == -1) {
					send(sock, "You need to access an account using the serve command first.\n", 255, 0);
					sem_post(&semaphore);
					continue;
				}
				command = strtok(NULL, " ");
				char * endptr;
				errno = 0;
				double amt = strtod(command, &endptr);
				if ((amt == 0 || amt < 0 && (errno != 0 || command == endptr)) || amt < 0) {
					char doubleError[] = "Please input a valid double -- withdraw <double Amount>. E.g. withdraw 100.05\n";
					send(sock , doubleError , 255, 0);
					continue;
				}
				char buff[255];
				double total = withdraw(amt, servedAccount);
				if (total == -1) {
					sprintf(buff, "Failed to withdraw %f, balance too low.\n", amt);
				} else {
					sprintf(buff, "Succesfully withdrew %f!, Current Total: %f\n", amt, total);
				}
				send(sock , buff , 255, 0);
			
			} else {
				char error[] = "That is not a valid command. Use one of the following:\ncreate <accountName>\nserver <accountName>\ndeposit <double Amount>\nwithdraw <double Amount>\nquery\nend\nquit";
				send(sock, error, 255, 0);
			}
			client_message[0] = '\0';
			sem_post(&semaphore);
		}
	}
    
         
    return 0;
    
}



void * signal_thread(void * arg) {


	while (1) {
		int caught_signal;
	
		sigwait(&mask, &caught_signal);
		pthread_mutex_lock(&mutex);
			int i;
		switch(caught_signal) {
		case SIGINT:
			puts("Server manually terminated!");
			int count = threadCount;
			for (i = 0; i < threadCount; i++) {
				sem_wait(&semaphore);
			}
			for(i = 0; i<clientCount; i++){
				send(socketIds[i], "ServerShutDown", 255, 0);
			}
			for (i = 0; i < threadCount; i++) {
				sem_post(&semaphore);
			}
			pthread_mutex_unlock(&mutex);
			for (i =0 ; i < count ; i++) {
				pthread_join(threadIds[i], NULL);
			}
			exit(0);
			break;
		case SIGALRM:
			for (i = 0; i < threadCount; i++) {
				sem_wait(&semaphore);
			}
			int j;
			printf("Account Name,\tBalance,\tIn Service?\n");
			for (j = 0; j < numberOfElements; j++) {
				char* inService = "";
				if(accounts[j].inUse == 1){
					inService = "IN SERVICE";
				} else {
					inService = "NOT IN SERVICE";
				}
				printf("%s,\t%f,\t%s\n", accounts[j].accountName, accounts[j].balance, inService);
			}
			for (i = 0; i < threadCount; i++) {
				sem_post(&semaphore);
			}
			pthread_mutex_unlock(&mutex);
			break;
		default:
			break;
		}
	}
}

int main(int varc, char* argv[])
{
    /* do the necessary setup, i.e. bind() and listen()... */

    if(varc != 2){
    	printf("ERROR: wrong amount of command line arguments");
	return 1;
    }
	int rc;
    pthread_t signal_tid;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGALRM);
	rc = pthread_sigmask(SIG_BLOCK, &mask, NULL);
	
	if (rc != 0) 
		puts("Sig mask error");

	rc = pthread_create(&signal_tid, NULL, signal_thread, &mask);

	if (rc!= 0)
		puts("Sig thread error");

	


	struct itimerval timer;
 	timer.it_value.tv_sec = 15;
 	timer.it_value.tv_usec = 0;
 	timer.it_interval.tv_sec = 15;
 	timer.it_interval.tv_usec = 0;
 	setitimer (ITIMER_REAL, &timer, NULL);
	sem_init(&semaphore, 0,0);

int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     int sockfd, newsocket, length;
     int port =  atoi (argv[1]);
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
        newsocket = accept(socket_desc, (struct sockaddr *) &client, &length);
		
		//pthread_mutex_lock(&mutex);
		if (newsocket == -1) {
			perror("Error at accept..");
		}
		printf("Accepted new client connection!\n");
		socketIds[clientCount++] = newsocket;
        if( pthread_create( &thread_id , NULL ,  handle_connection , &newsocket) < 0)
        {
            perror("could not create thread");
            return 1;
        } else {
			threadIds[threadCount++] = thread_id;
		}
        sem_post(&semaphore);
		//pthread_mutex_unlock(&mutex);
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
		
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
