all:
	gcc -w utils.h tests.h test.c -o test
	gcc -w client.c -o client
	gcc -w server.c -o server
	rm output*
