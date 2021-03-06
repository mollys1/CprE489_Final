#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

#define TOTAL_BYTES 992
#define BYTES_PER_PACKET 4

int main( int argc, const char* argv[] )
{
	int bufferSize = 40, err = 0, n = 0;
	char buffer[bufferSize];
	struct sockaddr_in ControlAddr, Process0_Addr, Process1_Addr, Process2_Addr;
	int controlSocket, process0Socket, process1Socket, process2Socket;
	int processSockets[3];
	char* generatedBytes = generateBytes();
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
	//also do an initial write (have to do this to get all the connections to work...)
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
		checkError(pipe(pipefd[i]), "Error creating pipe");
		cpid = fork();
		checkError(cpid, "Error on fork");
		if (cpid == 0)
		{
			char buf[BYTES_PER_PACKET + 1];
			char errorString[40];
			//read from pipe
			err = read(pipefd[i][0], buf, BYTES_PER_PACKET);
			buf[BYTES_PER_PACKET] = '\0';
			while(err)
			{	
				sprintf(errorString, "Process %d error on pipe read", i);
				checkError(err, errorString);
				//write to TCP socket
				err = write(processSockets[i], buf, strlen(buf));
				sprintf(errorString, "Process %d socket error on write", i);
				checkError(err, errorString);
				err = read(pipefd[i][0], buf, BYTES_PER_PACKET);
				buf[BYTES_PER_PACKET] = '\0';
			}
			exit(0);
		}
	}
	FILE *outputFile;
        outputFile = fopen("output_client.txt", "w");
        if (outputFile == NULL)
        {
                printf("Error opening file\n");
                exit(0);
        }

	char* dataSegment = (char *) malloc(sizeof(char) * (BYTES_PER_PACKET + 1));
	int dataSeqNum = 0, byte = 0;
	char* dataSeqNumBuf = (char *) malloc(sizeof(char) * (40));
	for (dataSeqNum = 0; dataSeqNum < (TOTAL_BYTES/BYTES_PER_PACKET);)
	{
		for (i = 0; i < 3 && dataSeqNum < (TOTAL_BYTES/BYTES_PER_PACKET); i++)
		{
			//write sequence number to control socket
			sprintf(dataSeqNumBuf, "Data Seq Num: %d, Subflow Seq Num: %d, Flow: %d", dataSeqNum, dataSeqNum/3, i);		
			fprintf(outputFile, "%s\n", dataSeqNumBuf);
			err = write(controlSocket, dataSeqNumBuf, strlen(dataSeqNumBuf));
			//write data segment to pipe
			strncpy(dataSegment, generatedBytes + byte, BYTES_PER_PACKET);
			fprintf(outputFile, "%s\n", dataSegment);
			err = write(pipefd[i][1], dataSegment, strlen(dataSegment));
			checkError(err, "Error on pipe write");
			dataSeqNum++;
			byte += 4;
		}
	}

	close(outputFile);
	//close pipes
	close(pipefd[0]);
	close(pipefd[1]);
	close(pipefd[2]);
	
	//close all sockets
	close(controlSocket);
	close(processSockets[0]);
	close(processSockets[1]);
	close(processSockets[2]);

	return 0;
}
