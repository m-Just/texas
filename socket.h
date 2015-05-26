#ifndef SOCKET_H
#define SOCKET_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int establishConnection(char* serverName, unsigned short serverPort, char* hostName, unsigned short hostPort) {
	int s, connected = 0;
	struct sockaddr_in sa;
	struct sockaddr_in sh;
	struct in_addr server;
	struct in_addr host;
	
	inet_aton(serverName, &server);
	inet_aton(hostName, &host);

	sa.sin_family = AF_INET;
	sa.sin_port = htons(serverPort);
	sa.sin_addr.s_addr = server;
	sh.sin_family = AF_INET;
	sh.sin_port = htons(hostPort);
	sh.sin_addr.s_addr = host;

	s = socket(AF_INET, SOCK_STREAM, 0);
	bind(s, (struct sockaddr*) &sh, sizeof(struct sockaddr_in));

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
