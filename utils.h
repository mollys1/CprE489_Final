#include <stdio.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define REPETITIONS 16
//checks if returnVal less than 0, if so displays error & ends program
void checkError(int returnVal, char* message)
{
	if (returnVal < 0)
	{
		perror(message);
		exit(0);
	}
}

char* generateBytes()
{
	int i = 0;
	char* generated = (char *) malloc(993);
	for (i = 0; i < REPETITIONS; i++)
	{
		strcat(generated, "0123456789");
		strcat(generated, "abcdefghijklmnopqrstuvwxyz");
		strcat(generated, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	}
	return generated;
}

struct sockaddr_in createSockAddr(char* ip, int port)
{
	struct sockaddr_in addr;
	bzero((char *) &addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	return addr;
}

int createClientSocket(struct sockaddr_in sockAddr)
{
	int sock = 0;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	checkError(sock, "Error creating socket");
	checkError(connect(sock, (struct sockaddr*) &sockAddr, sizeof(sockAddr)), "Error connecting to server");
	return sock;
}

int createServerSocket(struct sockaddr_in sockAddr)
{
	int connSocket =  socket(AF_INET, SOCK_STREAM, 0);
	int addrLength = sizeof(sockAddr);
	checkError(connSocket, "Error creating connection socket");
	//binding socket
	int err = bind(connSocket, (struct sockaddr *)  &sockAddr, sizeof(sockAddr));
	checkError(err, "Error binding connection socket");	
	err = listen(connSocket, 5);
	checkError(err, "Error listening on connection socket");
		
	int s = accept(connSocket, &sockAddr, &addrLength);
	checkError(s, "Error on accept");
	return s;
}
