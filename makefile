all: bankingServer.c bankingClient.c
	gcc -g -O0 bankingServer.c -lpthread -o bankingServer
	gcc -g -O0 bankingClient.c -lpthread -o bankingClient
bankingServer.o: bankingServer.c
	gcc -g -c bankingServer.c -o bankingServer.o
bankingClient.o: bankingClient.c
	gcc -g -c bankingClient.c -o bankingClient.o
clean:
	rm -rf bankingServer bankingClient.o bankingServer.o bankingClient.o bankingClient
