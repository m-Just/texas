#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "socket.h"
#include "constant.h"
#include "conversion.h"
#include "opponent.h"
#include "Card.h"
#include<algorithm>
#include<string.h>
using namespace std;
#include <time.h>

extern ANAOPP opp[];

int fd;// socket id code
double pre_flop_rate[2][MAX_PLAYER_NUM + 1][52][52];
int stage_minus = 0; //stage_minus == 1 means that the information may be from the last stage. 
int stage;

struct player
{
	int pid, jetton, money;
}button, sblind, bblind, nor[10], my;//nor[0].pid is the number of other players.
//nor: other players.

int ConnectAndReg(int argc, char* agrv[]) ///* connect to server and register*/
{
	char* serverName;
	char* hostName;
	unsigned short serverPort, hostPort, id;
	serverName = agrv[1];
	serverPort = atoi(agrv[2]);
	hostName = agrv[3];
	hostPort = atoi(agrv[4]);
	id = atoi(agrv[5]);
	fd = establishConnection(serverName, serverPort, hostName, hostPort);

	if (fd != -1) printf("Connection established.\n");
	else { printf("Connection failure. Program Abort.\n"); exit(1); }


	strcpy(tmp, PNAME);
	reg(id, fd, tmp);
	return 1;
}

//read part-----------------------------------------------------------------------------
int hold[5], com[10];

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

void Mate1Action(int round, int stage)
{
	double avrg = 0;
	const double AVGC = 1.6;
	double draw_line = 1.0 / plnum;
	rate R;
	if (com[0] >= 3) R = win_rate(hold + 1, com + 1, com[0], plnum); 
	else
	{
		R.first = pre_flop_rate[0][plnum][hold[1]][hold[2]];
		R.second = pre_flop_rate[1][plnum][hold[1]][hold[2]];
	}
	int maxbet = 0;
	for (int i = 0; i < 8; i++)
	{
		avrg += opp[i].avrgBet * (opp[i].jetton[round - 1] + opp[i].money[round - 1]);
		maxbet = max(maxbet, opp[i].bet[round][stage - 1]);
	}
	avrg /= 28000;
	int mebet = (int)(avrg * pow(2.0 ,(R.second - draw_line * 1.5) / draw_line * 3));
	if (round < 10) mebet = 0;
#ifdef _TEST
	printf("/*********round : %d ***********/", round);
	printf("player: %d\n", plnum);
	print_Card(hold + 1, 2, "hold");
	print_Card(com + 1, com[0], "public card");
	printf("average:%.0lf draw: %lf win: %lf maxbet: %7d mebet:%7d\n", avrg, R.first, R.second, maxbet, mebet);
#endif
	if (maxbet > mebet) action(FOLD, 0, fd); else action(RAISE, (int)(mebet - maxbet), fd);
}

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
				button.pid = SGI;	button.jetton = SGI;	button.money = SGI;
				plnum ++;
			}else if(strcmp(msg, "small") == 0){
				IOW;
				sblind.pid = SGI;	sblind.jetton = SGI;	sblind.money = SGI;
				plnum ++;	
			}else if(strcmp(msg, "big") == 0){
				IOW;
				bblind.pid = SGI;	bblind.jetton = SGI;	bblind.money = SGI;
				plnum ++;	
			}else{
				nor[0].pid ++;
				int x = nor[0].pid;
				nor[x].pid = atoi(msg);  nor[x].jetton = SGI;	nor[x].money = SGI;
				plnum ++;
			}
		}	
	}
	if(strcmp(msg, "game-over") == 0)return 2;
	if(strcmp(msg, "blind/") == 0){
		int num = 0;
		IOW; sblind.jetton -= SGI;
		while(1){
			get_word(msg, fd);
			if(strcmp(msg, "/blind") == 0)return 3;
			else bblind.jetton -= SGI;
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
				IOW;	
				pot = SGI;
			}else{
				done[0].pid++;
				int x = done[0].pid;
				change_to_num(msg, &num); done[x].pid = num;
				done[x].jetton = SGI;	done[x].money = SGI;	done[x].bet = SGI;
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
//read part-----------------------------------------------------------------------------

//main logic part--------------------------------------------------------------
int leastraise, mybet;

int get_uplim(double winrate, int jet, int mybet)
{
	int tmp = pot - mybet, f = 0;
	int i;
	struct getbet{
		int pid, bet;
	}pl[10];
	memset(pl, 0, sizeof(pl));
	pl[1].pid = button.pid;
	pl[2].pid = sblind.pid;
	pl[0].pid = 2;
	if(plnum > 2)pl[3].pid = bblind.pid, pl[0].pid = 3;
	for(i = 1; i <= nor[0].pid; i++){
		pl[0].pid++;
		pl[pl[0].pid].pid = nor[i].pid;
	}
	for(i = 1; i <= done[0].pid; i++){
		int id;
		for(int j = 1; j <= pl[0].pid; j++)
			if(pl[j].pid == done[i].pid){
				id = j; break;
			}
		pl[id].bet = done[i].bet;
	}
	for(i = 1; i <= pl[0].pid; i++){
		int id = hash(pl[i].pid);
		if(pl[i].pid != my.pid){
			if(opp[id].avrgBet){
				if(pl[i].bet < opp[id].avrgBet)tmp += opp[id].avrgBet - pl[i].bet;
			}else if(pl[i].bet < BIG_BLIND)tmp += BIG_BLIND - pl[i].bet;
		}
	} 
	double para = 1.0;
	if((double)jet/START_JETTON < 1.0)para = (double)jet/START_JETTON;
	int ans = ((double)tmp * winrate * para) / (1.0 - winrate * para) + 0.5;
	return ans;
	//(tmp + n) * winrate * jet/START_JETTON = n
	//tmp*r*para = - n*r*para + n
	//tmp*r*para / (1 - r*para) = n
}


int get_handnut()
{
	Card7 get_handnut;
	int size = -1;
	for(int i = 1; i <= hold[0]; i++){
		size++;
		get_handnut.card[size].color = colorof(hold[i]);
		get_handnut.card[size].val = pointof(hold[i]);
	}
	for(int i = 1; i <= 5; i++){
		size++;
		int tmp = com[i];
		if(i > com[0])tmp = 0;
		get_handnut.card[size].color = colorof(tmp);
		get_handnut.card[size].val = pointof(tmp);
	}
	get_handnut.get_level();
	return get_handnut.level;
}

struct getraise
{
	int pid, up;
};

bool cmp(getraise a, getraise b)
{
	return a.up < b.up;
}

int get_raise(int round, int nowbet, int mostbet)
{
	int follow = 0, f = 0, upfol;
	getraise ret[10];
	ret[0].pid = 0;
	for(int i = 1; i <= done[0].pid; i++){
		if (stage_minus && f){
			if (stage > 2)upfol = estFold(done[i].pid, com + 1, stage + 1, stage - 1, round);
			else upfol = 0;	
		}
		else{
			if (done[i].pid != my.pid) upfol = estFold(done[i].pid, com + 1, com[0], stage, round);
		}
		if (done[i].pid == sblind.pid) f = 1;
		if(upfol == 0)upfol = leastraise + nowbet;
		if(upfol == -1)upfol = 2147483647;
		ret[0].pid++;
		ret[ret[0].pid].up = upfol;
		ret[ret[0].pid].pid = done[i].pid;
	}
	int maxn = 0, ans = 0;
	sort(ret + 1, ret + 1 + ret[0].pid, cmp);
	for(int i = 1; i <= ret[0].pid; i++){
		if(ret[i].up < leastraise + nowbet)continue;	
		if(ret[i].up == 2147483647)break;
		int id, tmp = 0;
		for(int j = i; j <= ret[0].pid; j++){
			for(int k = 1; k <= done[0].pid; k++)
				if(done[k].pid == ret[i].pid){
					id = k; break;
				}
			tmp += ret[i].up - done[id].bet;
		}
		if(tmp > maxn)maxn = tmp, ans = ret[i].up - nowbet;
	}
	if(maxn < ans * (plnum / 4))ans = 0;
	if(ans + nowbet > mostbet)ans = mostbet - nowbet;
	return ans;
}
//main logic part--------------------------------------------------------------

//before action--------------------------------------------------------------------
void pre_action(int x, int round)
{
	//get stage and init(leastraise)
	stage_minus = 0;
	if(x == BLIND_MSG)stage = PREFLOP, leastraise = BIG_BLIND;
	if(x == HOLD_MSG) stage = PREFLOP, leastraise = BIG_BLIND;
	if(x == COM_CARDS_MSG){
		if(com[0] == 3) stage = FLOP;
		if(com[0] == 4) stage = TURN;
		if(com[0] == 5) stage = RIVER;
		stage_minus = 1;
		leastraise = BIG_BLIND;
	}
	//get stage and init(leastraise)
	
	
	if(x == SEAT_MSG && round == 0){
		int i = 0;
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
	if(x == SEAT_MSG){
		int i;
		i = hash(button.pid);			
		opp[i].money[round] = button.money;
		opp[i].jetton[round] = button.jetton;

		i = hash(sblind.pid);
		opp[i].money[round] = sblind.money;
		opp[i].jetton[round] = sblind.jetton;
				
		if(plnum > 2){
			i = hash(bblind.pid);
			opp[i].money[round] = bblind.money;
			opp[i].jetton[round] = bblind.jetton;
		}
		for(int j = 1; j <= nor[0].pid; j++){
			i = hash(nor[j].pid);
			opp[i].money[round] = nor[j].money;
			opp[i].jetton[round] = nor[j].jetton;
		}
	}
	if(x == INQUIRE_MSG || x == NOTIFY_MSG){
		int i;
		int f = 0;
		for(i = 1; i <= done[0].pid; i++){
			if(done[i].pid == my.pid){
				my.jetton = done[i].jetton;
				my.money = done[i].money;
				mybet = done[i].bet;
			}
			
			if(stage_minus == 1 && f == 1){
				int bet = done[i].bet;
				updateData(done[i].pid, done[i].action, bet, done[i].jetton, done[i].money, stage - 1, round);
			}
			else{
				int bet = done[i].bet;
				updateData(done[i].pid, done[i].action, bet, done[i].jetton, done[i].money, stage, round);
			}
			if(done[i].action == RAISE && done[i].pid != my.pid){
				if(leastraise < done[i].bet - done[i + 1].bet)
					leastraise = done[i].bet - done[i + 1].bet;
			}
			if(done[i].pid == sblind.pid)f = 1;
		}
		stage_minus = 0;
	}
	if(x == SHOW_MSG){
		int i;
		for(i = 1; i <= rank[0].pid; i++){
			updateData(rank[i].pid, SHOW, rank[0].nut_hand*10+rank[i].nut_hand, -1, -1, POT_WIN, round);
		}
	}
}
//before action--------------------------------------------------------------------


int main(int argc, char* agrv[]) {

#ifdef WRITE_IN_FILE
	FILE * fout = freopen("melog.txt", "w", stdout);
#endif
	
	ConnectAndReg(argc, agrv);
	read_pre_flop(pre_flop_rate[1], pre_flop_rate[0]);

	my.pid = atoi(agrv[5]);
	my.jetton = START_JETTON;
	my.money = START_MONEY;

	/* main round loop */
	int round, stage, stagenum;
	for (round = 0; round < MAX_ROUND; round++) {
		mybet = 0;
		leastraise = BIG_BLIND;
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
			pre_action(x, round);
			if (x == POT_MSG)
			{
				compute(round);
				break;
			}

			//action
			//if (x == INQUIRE_MSG) Mate1Action(round, stage);
			
			if(x == INQUIRE_MSG){
#ifdef WRITE_IN_FILE
				fprintf(fout, "com[0] = %d  stage = %d\n\n", &com[0], &stage);
#endif
				int i, act = 0, uplim, needcall = 0;//0: no need call  1: need call
				if(done[1].bet > mybet)needcall = 1;
				else needcall = 0;
				double winrate;
				if(stage == 1){//before flop
					winrate = pre_flop_rate[1][plnum][hold[1]][hold[2]];	
					uplim = get_uplim(winrate, my.jetton, mybet);
#ifdef WRITE_IN_FILE
				fprintf(fout, "winrate = %d  mybet = %d  uplim = %d\n\n", &winrate, &mybet, &uplim);
#endif
					if(needcall == 0)action(CHECK, 0, fd);
					else{
						if(done[1].bet > uplim)action(FOLD, 0, fd);
						else action(CALL, 0, fd), mybet = done[1].bet;
					}
				}
				if(stage >= 2){
					int f = 0;
					double rel_winrate;
					winrate = win_rate(hold + 1, com + 1, com[0], plnum).second;
					rel_winrate = winrate;
					int hold_poker = get_handnut();//present nut hand
					for(i = 1; i <= done[0].pid; i++){
						int tmp = -1;
						if (stage_minus == 1 && f) tmp = estHand(done[i].pid, com + 1, com[0], stage - 1, round);
						else tmp = estHand(done[i].pid, com + 1, com[0], stage, round);
						if (done[i].pid == sblind.pid) f = 1;
						if(tmp != -1 && tmp != 0){
							double pos = REL_WINRATE_POS;
							if(hold_poker < tmp)pos = -REL_WINRATE_POS;
							rel_winrate *= (1 + (REL_WINRATE_MULT * (hold_poker - tmp) - pos));
						}
					}
					double ret = 1.0;
					for(int i = 1; i <= 8 - plnum; i++)ret *= 0.9;
					uplim = get_uplim(rel_winrate, my.jetton, mybet);
					if(rel_winrate * ret > RAISELEVEL){
						int upraise = get_raise(round, done[1].bet, uplim);
						if(upraise >= leastraise)action(RAISE, upraise, fd), leastraise = upraise, mybet = done[1].bet + upraise;
						else{
							if(needcall == 0)action(CHECK, 0, fd);
							else{
								if(done[1].bet > uplim)action(FOLD, 0, fd);
								else action(CALL, 0, fd), mybet = done[1].bet;
							}
						}
					}else{
						if(needcall == 0)action(CHECK, 0, fd);
						else{
							if(done[1].bet > uplim)action(FOLD, 0, fd);
							else action(CALL, 0, fd), mybet = done[1].bet;
						}
					}
				}
			}
		}
	}

#ifdef WRITE_IN_FILE
	fclose(fout);
#endif
	return 0;
}
