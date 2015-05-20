#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int establishConnection(char* serverName, unsigned short portNum) {
	int s;
	struct sockaddr_in sa;
	struct in_addr server;
	
	inet_aton(serverName, &server);

	sa.sin_family = AF_INET;
	sa.sin_port = htons(portNum);
	sa.sin_addr = server;

	s = socket(AF_INET, SOCK_STREAM, 0);
	connect(s, (struct sockaddr*) &sa, sizeof(struct sockaddr_in));
	return s;
}

// int read (int s, void *buf, int len);

// int write(int s, void *buf, int len);

void disconnect(int s) {
	close(s);
}
