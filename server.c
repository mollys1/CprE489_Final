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
	int n;
	//set up sockaddr objects
	char ip[] = "127.0.0.1";
	ControlAddr = createSockAddr(ip, port++);
	controlSocket = createServerSocket(ControlAddr);
	Process0_Addr = createSockAddr(ip, port++);
	socket0 = createServerSocket(Process0_Addr);
	Process1_Addr = createSockAddr(ip, port++);
	socket1 = createServerSocket(Process1_Addr);
	Process2_Addr = createSockAddr(ip, port);
	socket2 = createServerSocket(Process2_Addr);
	//set up sockets
	//read from sockets
	int i = 0;
	for (i = 0; i < 2; i++)
	{
		n = read(controlSocket, controlBuff, BUFFER_SIZE);	
		checkError(n, "Error on read");
		n = read(socket0, buff0, BUFFER_SIZE);
		checkError(n, "Error on read");
		n = read(socket1, buff1, BUFFER_SIZE);
		checkError(n, "Error on read");
		n = read(socket2, buff2, BUFFER_SIZE);
		checkError(n, "Error on read");
		//print buffer
		printf("Control: %s\n", controlBuff);
		printf("0: %s\n", buff0);	
		printf("1: %s\n", buff1);	
		printf("2: %s\n", buff2);
	}
	//close sockets
	close(controlSocket);
	close(socket0);	
	close(socket1);	
	close(socket2);	
}	

