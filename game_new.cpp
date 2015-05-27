int hold[5], com[10];
#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "constant.h"
#include "conversion.h"
#include "opponent.h"
#include "Card.h"
#include<string.h>
#define TEST
#ifdef TEST
#include <time.h>
#endif


extern ANAOPP opp[MAX_PLAYER_NUM];

char * strrev(char *str)
{
  char *right = str;
  char *left = str;
  char ch;
  while (*right)   right++;
  right--;
  while (left < right)
  {
    ch = *left;
    *left++ = *right;
    *right-- = ch;
  }
  return(str);
}


void get_word(char msg[], int fd)
{
    char tmp[2];
    int flag = 0;
    msg[0] = 0;
    while(1){
        while(read(fd, tmp, sizeof(tmp[0])) == 0) printf("0");
        if(tmp[0] == ' ' || tmp[0] == '\n'){
            if(flag == 1){
#ifdef TEST
			printf("%s\n",msg);	
#endif
			break;
	    }
        }else{
			char tmp1[2] = {0, 0};
			tmp1[0] = tmp[0];
			strcat(msg, tmp1);
			flag = 1;
		}
    }
}

int change_to_num(char msg[], int *number)// return -1: not number    return 0: is number
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
        }else if(i != l - 1 || tmp[i] != ':')return -1;
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
            char tmp[2] = {0}; tmp[0] = ret + 48;
            strcat(num1, tmp);
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

void reg(int num, int fd)
{
	char msg[50] = {0};
	char num1[10] = {0};
    while(num > 0){
        int ret = num%10;
        char tmp[2] = {0}; tmp[0] = ret + 48;
        strcat(num1, tmp);
        num /= 10;    
    }
    strrev(num1);
    strcpy(msg, "reg: ");
    strcat(msg, num1);
    strcat(msg, " hudebudeliao need_notify \n");
    write(fd, msg, sizeof(msg));

   // reg: pid pname need_notify eol
}

	//action(kind, bet number, socket);
	//get_word(&a_word_name, socket);
	//change_to_num(a_word_name, &a_number_name);


struct player
{
	int pid, jetton, money;
}button, sblind, bblind, nor[10], my;//nor[0].pid is the number of other players.
//nor: other players.

struct player_in_game
{
	int pid, jetton, money, bet, action; //1:check  2:call  3:raise  4:all_in  5:fold  6:blind
}done[10]; //done[0].pid is the number of players.

struct player_rank
{
	int pid;
	int hold[5];
	int nut_hand;
}rank[10];//rank[0].pid is player number.

struct pot_win
{
	int pid, num;
}win[10];//win[0].pid is player number.

int plnum = 0; //player number
int pot = 0;

int get_msg(int fd)//1:seat_info  2:game_over  3:blind  4:hold  5:inquire  6:common cards  7:showdown  8:pot-win  9:notify
{
	char msg[25] = {0};
	get_word(msg, fd);
	if(strcmp(msg, "seat/") == 0){
		nor[0].pid = 0;
		plnum = 0;
		while(1){
			get_word(msg, fd);
			if(strcmp(msg, "/seat") == 0)return 1;
			if(strcmp(msg, "button:") == 0){
				int num = 0;
				get_word(msg, fd); change_to_num(msg, &num); button.pid = num;	
				get_word(msg, fd); change_to_num(msg, &num); button.jetton = num;
				get_word(msg, fd); change_to_num(msg, &num); button.money = num;	
				plnum ++;
			}else if(strcmp(msg, "small") == 0){
				int num = 0;
				get_word(msg, fd);
				get_word(msg, fd); change_to_num(msg, &num); sblind.pid = num;	
				get_word(msg, fd); change_to_num(msg, &num); sblind.jetton = num;
				get_word(msg, fd); change_to_num(msg, &num); sblind.money = num;
				plnum ++;	
			}else if(strcmp(msg, "big") == 0){
				int num = 0;
				get_word(msg, fd);
				get_word(msg, fd); change_to_num(msg, &num); bblind.pid = num;	
				get_word(msg, fd); change_to_num(msg, &num); bblind.jetton = num;
				get_word(msg, fd); change_to_num(msg, &num); bblind.money = num;
				plnum ++;	
			}else{
				int num = 0;
				nor[0].pid ++;
				int x = nor[0].pid;
				change_to_num(msg, &num); nor[x].pid = num;	
				get_word(msg, fd); change_to_num(msg, &num); nor[x].jetton = num;
				get_word(msg, fd); change_to_num(msg, &num); nor[x].money = num;	
				plnum ++;
			}
		}	
	}
	if(strcmp(msg, "game-over") == 0)return 2;
	if(strcmp(msg, "blind/") == 0){
		int num = 0;
		get_word(msg, fd); get_word(msg, fd); change_to_num(msg, &num); sblind.jetton -= num;
		while(1){
			get_word(msg, fd);
			if(strcmp(msg, "/blind") == 0)return 3;
			else{
				get_word(msg, fd); change_to_num(msg, &num); bblind.jetton -= num;
			}
		}
	}
	if(strcmp(msg, "hold/") == 0){
		char col[10];
		char poi[5];
		hold[0] = 0;
		while(1){
			get_word(msg, fd);
			if(strcmp(msg, "/hold") == 0)return 4;
			else{
				strcpy(col, msg);
				get_word(msg, fd); strcpy(poi, msg);
				hold[0]++;
				hold[hold[0]] = ctoi(col, poi);
			}
		}	
	}
	if(strcmp(msg, "inquire/") == 0){
		done[0].pid = 0;
		while(1){
			int num = 0;
			get_word(msg, fd);
			if(strcmp(msg, "/inquire") == 0)return 5;
			else if(strcmp(msg, "total") == 0){
				get_word(msg, fd); get_word(msg, fd);
				change_to_num(msg, &num);
				pot = num;
			}else{
				done[0].pid++;
				int x = done[0].pid;
				change_to_num(msg, &num); done[x].pid = num;
				get_word(msg, fd); change_to_num(msg, &num); done[x].jetton = num;
				get_word(msg, fd); change_to_num(msg, &num); done[x].money = num;
				get_word(msg, fd); change_to_num(msg, &num); done[x].bet = num;
				get_word(msg, fd);
				if(strcmp(msg, "check") == 0)done[x].action = 1;
				if(strcmp(msg, "call") == 0)done[x].action = 2;
				if(strcmp(msg, "raise") == 0)done[x].action = 3;
				if(strcmp(msg, "all_in") == 0)done[x].action = 4;
				if(strcmp(msg, "fold") == 0)done[x].action = 5;
				if(strcmp(msg, "blind") == 0)done[x].action = 6;
			}
		}
	}
	if(strcmp(msg, "flop/") == 0 || strcmp(msg, "turn/") == 0 || strcmp(msg, "river/") == 0){
		char col[10];
		char poi[5];
		com[0] = 0;
		while(1){
			get_word(msg, fd);
			if(strcmp(msg, "/flop") == 0 || strcmp(msg, "/turn") == 0 || strcmp(msg, "/river") == 0)return 6;
			else{
				strcpy(col, msg);
				get_word(msg, fd); strcpy(poi, msg);
				com[0]++;
				com[com[0]] = ctoi(col, poi);
			}
		}
	}
	if(strcmp(msg, "showdown/") == 0){
		while(strcmp(msg, "/common") != 0)get_word(msg, fd);
		rank[0].pid = 0;
		while(1){
			get_word(msg, fd);
			if(strcmp(msg, "/showdown") == 0)return 7;
			else{
				int num, x;
				rank[0].pid++; x = rank[0].pid;
				get_word(msg, fd); change_to_num(msg, &num); rank[x].pid = num;
				int i;
				for(i = 1; i <= 2; i++){
					char col[10];
					char poi[5];
					get_word(msg, fd); strcpy(col, msg);
					get_word(msg, fd); strcpy(poi, msg);
					rank[x].hold[i] = ctoi(col, poi);
				}
				get_word(msg, fd);
				if(strcmp(msg, "HIGH_CARD") == 0)rank[x].nut_hand = 1;
				if(strcmp(msg, "ONE_PAIR") == 0)rank[x].nut_hand = 2;
				if(strcmp(msg, "TWO_PAIR") == 0)rank[x].nut_hand = 3;
				if(strcmp(msg, "THREE_OF_A_KIND") == 0)rank[x].nut_hand = 4;
				if(strcmp(msg, "STRAIGHT") == 0)rank[x].nut_hand = 5;
				if(strcmp(msg, "FLUSH") == 0)rank[x].nut_hand = 6;
				if(strcmp(msg, "FULL_HOUSE") == 0)rank[x].nut_hand = 7;
				if(strcmp(msg, "FOUR_OF_A_KIND") == 0)rank[x].nut_hand = 8;
				if(strcmp(msg, "STRAIGHT_FLUSH") == 0)rank[x].nut_hand = 9;
			}
		}
	}
	if(strcmp(msg, "pot-win/") == 0){
		win[0].pid = 0;
		while(1){
			get_word(msg, fd);
			if(strcmp(msg, "/pot-win") == 0)return 8;
			else{
				int num, x;
				win[0].pid++;
				x = win[0].pid;
				change_to_num(msg, &num); win[x].pid = num;
				get_word(msg, fd); change_to_num(msg, &num); win[x].num = num;
			}
		}
	}
	if(strcmp(msg, "notify/") == 0){
		done[0].pid = 0;
		while(1){
			int num = 0;
			get_word(msg, fd);
			if(strcmp(msg, "/notify") == 0)return 9;
			else if(strcmp(msg, "total") == 0){
				get_word(msg, fd); get_word(msg, fd);
				change_to_num(msg, &num);
				pot = num;
			}else{
				done[0].pid++;
				int x = done[0].pid;
				change_to_num(msg, &num); done[x].pid = num;
				get_word(msg, fd); change_to_num(msg, &num); done[x].jetton = num;
				get_word(msg, fd); change_to_num(msg, &num); done[x].money = num;
				get_word(msg, fd); change_to_num(msg, &num); done[x].bet = num;
				get_word(msg, fd);
				if(strcmp(msg, "check") == 0)done[x].action = 1;
				if(strcmp(msg, "call") == 0)done[x].action = 2;
				if(strcmp(msg, "raise") == 0)done[x].action = 3;
				if(strcmp(msg, "all_in") == 0)done[x].action = 4;
				if(strcmp(msg, "fold") == 0)done[x].action = 5;
				if(strcmp(msg, "blind") == 0)done[x].action = 6;
			}
		}
	}
}

int get_uplim(double winrate, int jet)
{
	int tmp = pot, f = 0;
	int i;
	for(i = 1; i <= done[0].pid; i++){
		if(f == 1){
			int x = hash(done[i].pid);
			if(opp[x].maxbet[0])tmp += opp[x].maxbet[0];
			else tmp += 30;
		}
		if(done[i].pid == sblind.pid)f = 1;
	}
	double para = 1.0;
	if((double)jet/START_JETTON < 1.0)para = (double)jet/START_JETTON;
	int ans = ((double)tmp * winrate * para) / (1.0 - winrate * para) + 0.5;
	return ans;
	//(tmp + n) * winrate * jet/START_JETTON = n
	//tmp*r*para = - n*r*para + n
	//tmp*r*para / (1 - r*para) = n
}

int main(int argc, char* agrv[]) {
	/* connect to server */
	int fd; // socket id code
	char* serverName;
	char* hostName;
	unsigned short serverPort, hostPort, id;

#ifdef TEST
	FILE * fout = freopen("melog.txt", "w", stdout);
	printf("%d\n",time(0));
#endif
	
	serverName = agrv[1];
	serverPort = atoi(agrv[2]);
	hostName = agrv[3];
	hostPort = atoi(agrv[4]);
	id = atoi(agrv[5]);

	printf("%s\n%d\n%s\n%d\n",serverName, serverPort, hostName, hostPort);
	fd = establishConnection(serverName, serverPort, hostName, hostPort);
	if (fd != -1) printf("Connection established.\n");
	else {  printf("Connection failure. Program Abort.\n"); return 1;}

	reg(id, fd);

	my.pid = id;
	my.jetton = START_JETTON;
	my.money = START_MONEY;

	/* main round loop */
	int round, flag = 0, mybet = 0;
	for (round = 0; round < MAX_ROUND; round++) {
		mybet = 0;
		hold[0] = 0;
		com[0] = 0;
		while(1){
			//read
			int x = get_msg(fd);
			
			//quit
			if(x == GAME_OVER_MSG){
				disconnect(fd);
				return 0;
			}
			
			//pre action
			if(x == SEAT_MSG){
				memset(opp, 0, sizeof(opp));
				int i = 1;
				opp[i].pid = button.pid;
				i++;
				opp[i].pid = sblind.pid;
				i++;
				if(plnum > 2)opp[i].pid = bblind.pid, i++;
				int j;
				for(j = 1; j <= nor[0].pid; j++){
					opp[i].pid = nor[j].pid;
					i++;
				}
			}
			if(x == INQUIRE_MSG || x == NOTIFY_MSG){
				int i;
				int stage = 1;
				if(com[0] > 0)stage += com[0] - 2;
				for(i = 1; i <= done[0].pid; i++){
					if(done[i].pid == my.pid){
						my.jetton = done[i].jetton;
						my.money = done[i].money;
						mybet = done[i].bet;
					}
					int bet = 0;
					if(done[i].action == CHECK || done[i].action == ALLIN || done[i].action == FOLD);
					else bet = done[i].bet;
					updateData(done[i].pid, done[i].action, bet, done[i].jetton, done[i].money, stage, round);
				}
				//for test
				action(FOLD, 0, fd);
				//test over
			}
			if(x == SHOW_MSG){
				int i;
				for(i = 1; i <= rank[0].pid; i++){
					updateData(rank[i].pid, SHOW, rank[i].nut_hand, -1, -1, POT_WIN, round);
				}
			}
			if(x == POT_MSG){
				int i;
				for(i = 1; i <= win[0].pid; i++){
					updateData(win[i].pid, POT, /*num*//*for test*/0, -1, -1, POT_WIN, round);
				}
			}
			
			//action
			if(x == INQUIRE_MSG){
				int i, act = 0, uplim, needcall = 0;//0: no need call  1: need call
				if(done[1].bet > mybet)needcall = 1;
				int stage = 1;
				if(com[0] > 0)stage += com[0] - 2;
				double winrate;
				if(stage == 1){//before flop
					if(hold[0] == 0)winrate = 1.0/(double)plnum;
					else{
						Card pubc[6], handc[3];
						change_to_Card(pubc, handc, hold, com);
					 	winrate = win_rate(handc, pubc, com[0], plnum).second;	
					}
					uplim = get_uplim(winrate, my.jetton);
					if(needcall == 0)action(CHECK, 0, fd);
					else{
						if(done[1].bet > uplim)action(FOLD, 0, fd);
						else action(CALL, 0, fd);
					}
				}
				if(stage == 1){
					
				}
			}
			
		}
	}

#ifdef TEST
	fclose(fout);
#endif
	return 0;
}
