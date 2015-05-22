#include <stdio.h>
#include "socket.h"
#include <stdlib.h>

#define CHECK 1
#define CALL  2
#define RAISE 3
#define ALLIN 4
#define FOLD  5
#define BLIND 6

void get_word(char **msg, int fd)
{
    char tmp;
    bool flag = 0;
    *msg = 0;
    while(1){
        read(fd, tmp, sizeof(tmp));
        if(tmp == ' ' || tmp == '\n'){
            if(flag == 1)break;
        }else strcat(*msg, tmp), flag = 1;
    }
}

int change_to_num(char *msg, int *number)// return -1: not number    return 0: is number
{
    int l = strlen(msg), flag = 1;
    char tmp[l + 5];
    strcpy(tmp, msg);
    int num = 0, i;
    for(i = 0; i < l; i++){
        if(i == 0 && tmp[i] == '-')flag = -1;
        else if(tmp[i] >= '0' && tmp[i] <= '9'){
            num *= 10;
            num += tmp[i] - 48;
        }else return -1;
    }
    *number = num * flag;
    return 0;
}

void action(int x, int num, int fd)//1:check  2:call  3:raise num  4:all_in  5:fold
{
    char action_msg[25] = {0};
    if(x == CHECK){
        strcpy(action_msg, "check \n");
    }else if(x == CALL){
        strcpy(action_msg, "call \n");
    }else if(x == RAISE){
        char num1[10] = {0};
        while(num > 0){
            int ret = num%10;
            strcat(num1, ret + 48);
            num /= 10;    
        }
        strrev(num1);
        strcpy(action_msg, "raise ");
        strcat(action_msg, num1);
        strcat(action_msg, " \n");
    }else if(x == ALLIN){
        strcpy(action_msg, "all_in \n");
    }else if(x == FOLD){
        strcpy(action_msg, "fold \n");
    }
    write(fd, action_msg, sizeof(action_msg));
}


	//action(kind, bet number, socket);
	//get_word(&a_word_name, socket);
	//change_to_num(a_word_name, &a_number_name);

#include "conversion.h"

struct player
{
	int pid, jetton, money;
}button, sblind, bblind, nor[10];//nor[0].pid is the number of other players.
//nor: other players.

struct player_in_game
{
	int pid, jetton, money, bet, action; //1:check  2:call  3:raise  4:all_in  5:fold  6:blind
}done[10]; //done[0].pid is the number of players.

int hold[5], com[10];

int plnum = 0; //player number
int pot = 0;

int get_msg(int fd)//1:seat_info  2:game_over  3:blind  4:hold  5:inquire  6:
{
	char *msg;
	get_word(&msg, fd);
	if(strcmp(msg, "seat/") == 0){
		nor[0].pid = 0;
		plnum = 0;
		while(1){
			get_word(&msg, fd);
			if(strcmp(msg, "/seat") == 0)return 1;
			if(strcmp(msg, "button:") == 0){
				int num = 0;
				get_word(&msg, fd); change_to_num(msg, &num); button.pid = num;	
				get_word(&msg, fd); change_to_num(msg, &num); button.jetton = num;
				get_word(&msg, fd); change_to_num(msg, &num); button.money = num;	
				plnum ++;
			}else if(strcmp(msg, "small blind:") == 0){
				int num = 0;
				get_word(&msg, fd); change_to_num(msg, &num); sblind.pid = num;	
				get_word(&msg, fd); change_to_num(msg, &num); sblind.jetton = num;
				get_word(&msg, fd); change_to_num(msg, &num); sblind.money = num;
				plnum ++;	
			}else if(strcmp(msg, "big blind:") == 0){
				int num = 0;
				get_word(&msg, fd); change_to_num(msg, &num); bblind.pid = num;	
				get_word(&msg, fd); change_to_num(msg, &num); bblind.jetton = num;
				get_word(&msg, fd); change_to_num(msg, &num); bblind.money = num;
				plnum ++;	
			}else{
				int num = 0;
				nor[0].pid ++;
				int x = nor[0].pid;
				change_to_num(msg, &num); nor[x].pid = num;	
				get_word(&msg, fd); change_to_num(msg, &num); nor[x].jetton = num;
				get_word(&msg, fd); change_to_num(msg, &num); nor[x].money = num;	
				plnum ++;
			}
		}	
	}
	if(strcmp(msg, "game-over") == 0)return 2;
	if(strcmp(msg, "blind/") == 0){
		int num = 0;
		get_word(&msg, fd); get_word(&msg, fd); change_to_num(msg, &num); sblind.jetton -= num;
		while(1){
			get_word(&msg, fd);
			if(strcmp(msg, "/blind") == 0)return 3;
			else{
				get_word(&msg, fd); change_to_num(msg, &num); bblind.jetton -= num;
			}
		}
	}
	if(strcmp(msg, "hold/") == 0){
		char *col;
		char *poi;
		while(1){
			get_word(&msg, fd);
			if(strcmp(msg, "/hold") == 0)return 4;
			else{
				strcpy(col, msg);
				get_word(&msg, fd); strcpy(poi, msg);
				hold[0]++;
				hold[hold[0]] = ctoi(col, poi);
			}
		}	
	}
	if(strcmp(msg, "inquire/") == 0){
		while(1){
			int num = 0;
			get_word(&msg, fd);
			if(strcmp(msg, "/inquire") == 0)return 5;
			else if(strcmp(msg, "total") == 0){
				get_word(&msg, fd); get_word(&msg, fd);
				change_to_num(msg, &num);
				pot = num;
			}else{
				done[0].pid++;
				int x = done[0].pid;
				change_to_num(msg, &num); done[x].pid = num;
				get_word(&msg, fd); change_to_num(msg, &num); done[x].jetton = num;
				get_word(&msg, fd); change_to_num(msg, &num); done[x].money = num;
				get_word(&msg, fd); change_to_num(msg, &num); done[x].bet = num;
				get_word(&msg, fd);
				if(strcmp(msg, "ckeck") == 0)done[x].action = 1;
				if(strcmp(msg, "call") == 0)done[x].action = 2;
				if(strcmp(msg, "raise") == 0)done[x].action = 3;
				if(strcmp(msg, "all_in") == 0)done[x].action = 4;
				if(strcmp(msg, "fold") == 0)done[x].action = 5;
				if(strcmp(msg, "blind") == 0)done[x].action = 6;
			}
		}
	}
}



