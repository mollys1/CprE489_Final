#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tests.h"

int main( int argc, const char* argv[] )
{
	int bufferSize = 40, err = 0, n = 0;
	char buffer[bufferSize];
	struct sockaddr_in ControlAddr, Process0_Addr, Process1_Addr, Process2_Addr;
	int controlSocket, process0Socket, process1Socket, process2Socket;
	int processSockets[3];
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
	processSockets[0] = createClientSocket(Process0_Addr);
	sprintf(buffer, "Process 0 socket write");
	printf("%s\n", buffer);
	err = write(processSockets[0], buffer, strlen(buffer));
	checkError(err, "Process 0 socket error on write");
	
	printf("Process 1 connection\n");
	processSockets[1] = createClientSocket(Process1_Addr);
	sprintf(buffer, "Process 1 socket write");
	printf("%s\n", buffer);
	err = write(processSockets[1], buffer, strlen(buffer));
	checkError(err, "Process 1 socket error on write");
	
	printf("Process 2 connection\n");
	processSockets[2] = createClientSocket(Process2_Addr);
	sprintf(buffer, "Process 2 socket write");
	printf("%s\n", buffer);
	err = write(processSockets[2], buffer, strlen(buffer));
	checkError(err, "Process 2 socket error on write");
	
	//create pipes & children
	
	
	int pipefd[3][2];
	pid_t cpid;
	
	int i = 0;	
	for (i = 0; i < 3; i++)
	{
		char buf[40];
		char errorString[40];
		checkError(pipe(pipefd[i]), "Error creating pipe");
		cpid = fork();
		checkError(cpid, "Error on fork");
		if (cpid == 0)
		{
			close(pipefd[i][1]);
			read(pipefd[i][0], buf, 40);
			//write to TCP socket
			err = write(processSockets[i], buf, strlen(buffer));
			sprintf(errorString, "Process %d socket error on write", i);
			checkError(err, errorString);
			close(pipefd[i][0]);
			exit(0);
		}
	}
	char* hello = (char *) malloc(40);
	for (i = 0; i < 3; i++)
	{
		close(pipefd[i][0]);
		sprintf(hello, "Hello, child process %d", i);
		write(pipefd[i][1], hello, strlen(hello));
		close(pipefd[i][1]);
	}
	wait(NULL);

	
	
		//close all sockets
	close(controlSocket);
	close(process0Socket);
	close(process1Socket);
	close(process2Socket);
	return 0;
}
