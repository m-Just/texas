#include "socket.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include<time.h>

int establishConnection(char* serverName, unsigned short serverPort, char* hostName, unsigned short hostPort) {
	int s, connected = 0, opt;
	struct sockaddr_in sa;
	struct sockaddr_in sh;
	struct in_addr server;
	struct in_addr host;
	
	inet_aton(serverName, &server);
	inet_aton(hostName, &host);

	sa.sin_family = AF_INET;
	sa.sin_port = htons(serverPort);
	sa.sin_addr = server;
	sh.sin_family = AF_INET;
	sh.sin_port = htons(hostPort);
	sh.sin_addr = host;

	s = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bind(s, (struct sockaddr*) &sh, sizeof(struct sockaddr_in));

	int i;
	while(1) {
		if (!connect(s, (struct sockaddr*) &sa, sizeof(struct sockaddr_in))) {
			connected = 1; break;
		} else  printf("Connection failure. Reconnecting...\n");
		usleep(100 * 1000); 
	}

	if (connected) return s;
	else	       return -1;
}

void disconnect(int s) {
	close(s);
}
