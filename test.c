#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tests.h"

int main( int argc, const char* argv[] )
{
	char* generatedBytes = generateBytes();
	char* segment = (char *) malloc(5);
	int i = 0;
	for (i = 0; i < 40; i=i+4)
	{
		strncpy(segment, generatedBytes + i, 4);
		printf("Segment %d: %s\n", i, segment);
	}
	printf("%s\n", generatedBytes);
}
