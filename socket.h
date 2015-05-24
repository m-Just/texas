#ifndef SOCKET_H
#define SOCKET_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int establishConnection(char* serverName, unsigned short portNum) {
	int s, connected = 0;
	struct sockaddr_in sa;
	struct in_addr server;
	
	inet_aton(serverName, &server);

	sa.sin_family = AF_INET;
	sa.sin_port = htons(portNum);
	sa.sin_addr = server;

	s = socket(AF_INET, SOCK_STREAM, 0);

	int i;
	for (i = 1; i <= 5; i++) {
		if (!connect(s, (struct sockaddr*) &sa, sizeof(struct sockaddr_in))) {
			connected = 1; break;
		} else  printf("Connection failure. Reconnecting...\n");
	}

	if (connected) return s;
	else	       return -1;
}

// ssize_t read (int s, void *buf, int len);

// ssize_t write(int s, void *buf, int len);

void disconnect(int s) {
	close(s);
}

#endif
