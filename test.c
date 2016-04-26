#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tests.h"

int main( int argc, const char* argv[] )
{
	int answer = 0, junk1 = 0, junk2 = 0;
	char* string = "Banana is 12, 13, 14";
	sscanf(string, "Banana is %d, %d, %d", &answer, &junk1, &junk2);
	printf("Result %d\n", answer);
}
