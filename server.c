#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

#define BUFFER_SIZE 4
#define BYTES_PER_PACKET 4
#define TOTAL_BYTES 992

int main( int argc, const char* argv[] )
{
	if (argc < 2)
	{
		printf("Please provide a port number\n");
		return;
	}
	int port = atoi(argv[1]);;
	char controlBuff[1024], buff0[1024], buff1[1024], buff2[1024];
	struct sockaddr_in ControlAddr, Process0_Addr, Process1_Addr, Process2_Addr;
	int controlSocket, socket0, socket1, socket2;
	int dataSockets[3];
	int n, status = 0;
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
	n = read(controlSocket, controlBuff, 40);	
	checkError(n, "Error on read");
	printf("Control Initial Read: %s\n", controlBuff);
	n = read(dataSockets[0], buff0, 40);	
	checkError(n, "Error on read");
	printf("Data 1 Initial Read: %s\n", buff0);
	n = read(dataSockets[1], buff1, 40);	
	checkError(n, "Error on read");
	printf("Data 2 Initial Read: %s\n", buff1);
	n = read(dataSockets[2], buff2, 40);	
	checkError(n, "Error on read");
	printf("Data 3 Initial Read: %s\n", buff2);
	
	//set up sockets
	//fork children
	int pipefd[3][2];
	pid_t cpid;
	int i = 0;	
	for (i = 0; i < 3; i++)
	{
		char errorString[40];
		checkError(pipe(pipefd[i]), "Error creating pipe");
		cpid = fork();
		checkError(cpid, "Error on fork");
		if (cpid == 0)
		{
			char buf[40];
			//each child should read from tcp socket and write to pipe for parent
			n = read(dataSockets[i], buf, BUFFER_SIZE);
			int j;		
			while(n)
			{
				sprintf(errorString, "Process %d error on socket read", i);
				checkError(n, errorString);
				n = write(pipefd[i][1], buf, strlen(buf));
				sprintf(errorString, "Process %d error on pipe write", i);
				checkError(n, errorString);
				n = read(dataSockets[i], buf, BUFFER_SIZE);
			}
			exit(0);
		}
	}
	FILE *outputFile;
	outputFile = fopen("output_server.txt", "w");	
	if (outputFile == NULL)
	{
		printf("Error opening file\n");
		exit(0);
	}
	char buf[BYTES_PER_PACKET];
        int dataSeqNum = 0, subflowSeqNum = 0, flow = 0;
	char dataSeqNumBuf[1024];
        while (dataSeqNum < (TOTAL_BYTES/BYTES_PER_PACKET)-1)
        {        
                for (i = 0; i < 3 && dataSeqNum < (TOTAL_BYTES/BYTES_PER_PACKET)-1; i++)
                {
                	//read from control socket to get master sequence number
			n = read(controlSocket, dataSeqNumBuf, lengthDSS(dataSeqNum+1));
                	sscanf(dataSeqNumBuf, "Data Seq Num: %d, Subflow Seq Num: %d, Flow: %d", &dataSeqNum, &subflowSeqNum, &flow);
			fprintf(outputFile,"%s\n", dataSeqNumBuf);
        		       			
			//now read from pipes from the children
			n = read(pipefd[i][0], buf, BYTES_PER_PACKET);
			checkError(n, "Error on pipe read");
			fprintf(outputFile, "%s\n", buf);
			printf("%s\n", buf);
		}
	}
	close(outputFile);
	//close sockets
	close(pipefd[0]);
	close(pipefd[1]);
	close(pipefd[2]);

	close(controlSocket);
	close(socket0);	
	close(socket1);	
	close(socket2);	
	return(0);
}	

