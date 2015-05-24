#include <stdio.h>
#include "socket.h"
#include "conversion.h"
#include "opponent.h"

int main(int argc, char* agrv[]) {
	/* connect to server */
	int fd; // socket id code
	char* serverName;
	char* hostName;
	unsigned short serverPort, hostPort, id;
	
	serverName = agrv[1];
	serverPort = atoi(agrv[2]);
	hostName = agrv[3];
	hostPort = atoi(agrv[4]);
	id = atoi(agrv[5]);

	fd = establishConnection(serverName, serverPort);
	if (fd) printf("Connection established.\n");
	else {  printf("Connection failure. Program Abort.\n"); return 1;}

	/* main round loop */
	int round;
	for (round = 0; round < MAX_ROUND; round++) {
		
	}

	return 0;
}
