#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include "utils.h"

void testGenerateBytes()
{
	char* generated = generateBytes();
	printf("%s\n", generated);
	printf("%d\n", strlen(generated));
}

int testForks()
{
	int pipefd[4][2];
	pid_t cpid;
	
	int i = 0;	
	for (i = 0; i < 4; i++)
	{
		checkError(pipe(pipefd[i]), "Error creating pipe");
		cpid = fork();
		checkError(cpid, "Error on fork");
		if (cpid == 0)
		{
			FILE *fp;
			char *mode = "w";
			char fileName[10];
			char buf;
			sprintf(fileName, "output%d", i);
			fp = fopen(fileName, "w");
			if (fp == NULL) write(STDOUT_FILENO, "Error opening file\n", 1);
			close(pipefd[i][1]);
			while (read(pipefd[i][0], &buf, 1) > 0)
			{
				fprintf(fp, "%c", buf);
			}
			fprintf(fp, "\n"); 
			close(pipefd[i][0]);
			exit(0);
		}
	}
	char* hello = (char *) malloc(40);
	for (i = 0; i < 4; i++)
	{
		close(pipefd[i][0]);
		sprintf(hello, "Hello, child process %d", i);
		write(pipefd[i][1], hello, strlen(hello));
		close(pipefd[i][1]);
	}
	wait(NULL);
	return 1;
}

int testForks2()
{
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
			FILE *fp;
			char *mode = "w";
			char fileName[10];
			char buf[40];
			sprintf(fileName, "output%d", i);
			fp = fopen(fileName, "w");
			if (fp == NULL) write(STDOUT_FILENO, "Error opening file\n", 1);
			close(pipefd[i][1]);
			//while (read(pipefd[i][0], buf, 40) > 0)
			//{
			read(pipefd[i][0], buf, 40);
			fprintf(fp, "%s", buf);
			//}
			fprintf(fp, "\n"); 
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
	return 1;
}
