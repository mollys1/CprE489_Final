#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tests.h"

#define BUFFER_SIZE 1024

int main( int argc, const char* argv[] )
{
	if (argc < 2)
	{
		printf("Please provide a port number\n");
		return;
	}
	int port = atoi(argv[1]);;
	char controlBuff[BUFFER_SIZE], buff0[BUFFER_SIZE], buff1[BUFFER_SIZE], buff2[BUFFER_SIZE];
	struct sockaddr_in ControlAddr, Process0_Addr, Process1_Addr, Process2_Addr;
	int controlSocket, socket0, socket1, socket2;
	int dataSockets[3];
	int n;
	//set up sockaddr objects
	char ip[] = "127.0.0.1";
	ControlAddr = createSockAddr(ip, port++);
	controlSocket = createServerSocket(ControlAddr);
	Process0_Addr = createSockAddr(ip, port++);
	dataSockets[0] = createServerSocket(Process0_Addr);
	Process1_Addr = createSockAddr(ip, port++);
	dataSockets[1] = createServerSocket(Process1_Addr);
	Process2_Addr = createSockAddr(ip, port);
	dataSockets[2] = createServerSocket(Process2_Addr);
	//do initial read from all sockets (have to do this to get all the connections to work)
	n = read(controlSocket, controlBuff, BUFFER_SIZE);	
	checkError(n, "Error on read");
	printf("Control Initial Read: %s\n", controlBuff);
	n = read(dataSockets[0], buff0, BUFFER_SIZE);	
	checkError(n, "Error on read");
	printf("Data 1 Initial Read: %s\n", buff0);
	n = read(dataSockets[1], buff1, BUFFER_SIZE);	
	checkError(n, "Error on read");
	printf("Data 2 Initial Read: %s\n", buff1);
	n = read(dataSockets[2], buff2, BUFFER_SIZE);	
	checkError(n, "Error on read");
	printf("Data 3 Initial Read: %s\n", buff2);
	
	//set up sockets
	//fork children
	int pipefd[3][2];
	pid_t cpid;
	
	int i = 0;	
	for (i = 0; i < 3; i++)
	{
		checkError(pipe(pipefd[i]), "Error creating pipe");
		cpid = fork();
		checkError(cpid, "Error on fork");
		if (cpid == 0)
		{
			char buf[40];
			//each child should read from tcp socket and write to pipe for parent
			n = read(dataSockets[i], buf, BUFFER_SIZE);
			checkError(n, "Error on read");
			close(pipefd[i][0]);
			write(pipefd[i][1], buf, strlen(buf));
			close(pipefd[i][1]);
			exit(0);
		}
	}
	
	
	
	//now read from pipes from the children
	for (i = 0; i < 3; i++)
	{
		char buf[40];
		close(pipefd[i][1]);
		read(pipefd[i][0], buf, 40);
		printf("Process %d: %s\n", i, buf); 
		close(pipefd[i][0]);
	}
	//close sockets
	close(controlSocket);
	close(socket0);	
	close(socket1);	
	close(socket2);	
}	

