#include <stdio.h>
#include "socket.h"

int main(int argc, char* agrv[]) {
	/* connect to server */
	int socket;
	char* serverName;
	char* hostName;
	unsigned short serverPort, hostPort, id;
	
	serverName = agrv[1];
	serverPort = atoi(agrv[2]);
	hostName = agrv[3];
	hostPort = atoi(agrv[4]);
	id = atoi(agrv[5]);

	socket = establishConnection(serverName, serverPort);
	
	return 0;
}
