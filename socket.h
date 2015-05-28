#ifndef SOCKET_H
#define SOCKET_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int establishConnection(char* serverName, unsigned short serverPort, char* hostName, unsigned short hostPort) ;

// ssize_t read (int s, void *buf, int len);

// ssize_t write(int s, void *buf, int len);

void disconnect(int s);

void get_word(char msg[], int fd);

#endif
