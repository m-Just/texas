#include "socket.h"
#include "constant.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "conversion.h"

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

	printf("Setting up socket...\n");
	s = socket(AF_INET, SOCK_STREAM, 0)
	//setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bind(s, (struct sockaddr*) &sh, sizeof(struct sockaddr_in));
	while (bind(s, (struct sockaddr*) &sh, sizeof(struct sockaddr_in))) {
		printf("Socket bind failure. Retrying...\n");
	}

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

void get_word(char msg[], int fd)
{
	char tmp[2];
	int flag = 0;
	msg[0] = 0;
	while (1){
		while (read(fd, tmp, sizeof(tmp[0])) == 0)
		{
			usleep(1000 * 5);
			//printf("Nothing in socket for read!\n");
		}
		;
		if (tmp[0] == ' ' || tmp[0] == '\n'){
			if (flag == 1){
#ifdef _TEST
				printf("%s\n", msg);
#endif
				break;
			}
		}
		else{
			char tmp1[2] = { 0, 0 };
			tmp1[0] = tmp[0];
			strcat(msg, tmp1);
			flag = 1;
		}
	}
}

void action(int x, int num, int fd)//1:check  2:call  3:raise num  4:all_in  5:fold
{
	char action_msg[25] = { 0 };
	if (x == CHECK){
		strcpy(action_msg, "check \n");
	}
	else if (x == CALL){
		strcpy(action_msg, "call \n");
	}
	else if (x == RAISE){
		char num1[10] = { 0 };
		while (num > 0){
			int ret = num % 10;
			char tmp[2] = { 0 }; tmp[0] = ret + 48;
			strcat(num1, tmp);
			num /= 10;
		}
		strrev(num1);
		strcpy(action_msg, "raise ");
		strcat(action_msg, num1);
		strcat(action_msg, " \n");
	}
	else if (x == ALLIN){
		strcpy(action_msg, "all_in \n");
	}
	else if (x == FOLD){
		strcpy(action_msg, "fold \n");
	}
	write(fd, action_msg, sizeof(action_msg));
}

void reg(int num, int fd, char* name_and_notify)
{
	char msg[50] = { 0 };
	char num1[10] = { 0 };
	while (num > 0){
		int ret = num % 10;
		char tmp[2] = { 0 }; tmp[0] = ret + 48;
		strcat(num1, tmp);
		num /= 10;
	}
	strrev(num1);
	strcpy(msg, "reg: ");
	strcat(msg, num1);
	strcat(msg, " ");
	strcat(msg, name_and_notify);
	write(fd, msg, sizeof(msg));

	// reg: pid pname need_notify eol
}

int socket_get_int(int fd)
{
	char msg[10];
	get_word(msg, fd);
	return atoi(msg);
}
