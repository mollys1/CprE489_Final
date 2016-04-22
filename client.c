#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tests.h"

int main( int argc, const char* argv[] )
{
	int bufferSize = 1024, err = 0, n = 0;
	char buffer[bufferSize];
	struct sockaddr_in ControlAddr, Process0_Addr, Process1_Addr, Process2_Addr;
	int controlSocket, process0Socket, process1Socket, process2Socket;
	if (argc < 3)
	{
		printf("Please specify the IP address of the server and a port number\n");
		exit(0);
	}
	int port = atoi(argv[2]);
	//set up all the addresses
	printf("Port: %d\n", port);
	ControlAddr = createSockAddr(argv[1], port++);
	printf("Port: %d\n", port);
	Process0_Addr = createSockAddr(argv[1], port++);
	printf("Port: %d\n", port);
	Process1_Addr = createSockAddr(argv[1], port++);
	printf("Port: %d\n", port);
	Process2_Addr = createSockAddr(argv[1], port);
	printf("Port: %d\n", port);
	//create all connections
	printf("Control connection\n");
	controlSocket = createClientSocket(ControlAddr);
	sprintf(buffer, "Control socket write");
	printf("%s\n", buffer);
	err = write(controlSocket, buffer, strlen(buffer));
	checkError(err, "Control socket error on write");
	
	printf("Process 0 connection\n");
	process0Socket = createClientSocket(Process0_Addr);
	sprintf(buffer, "Process 0 socket write");
	printf("%s\n", buffer);
	err = write(process0Socket, buffer, strlen(buffer));
	checkError(err, "Process 0 socket error on write");
	
	printf("Process 1 connection\n");
	process1Socket = createClientSocket(Process1_Addr);
	sprintf(buffer, "Process 1 socket write");
	printf("%s\n", buffer);
	err = write(process1Socket, buffer, strlen(buffer));
	checkError(err, "Process 1 socket error on write");
	
	printf("Process 2 connection\n");
	process2Socket = createClientSocket(Process2_Addr);
	sprintf(buffer, "Process 2 socket write");
	printf("%s\n", buffer);
	err = write(process2Socket, buffer, strlen(buffer));
	checkError(err, "Process 2 socket error on write");
	///write to each socket

	

	
	
		//close all sockets
	close(controlSocket);
	close(process0Socket);
	close(process1Socket);
	close(process2Socket);
	return 0;
}
