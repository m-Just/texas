#ifndef OPPONENT_H
#define OPPONENT_H

#include "constant.h"

struct {           // database structure, for the analysis of the opponents
	int pid;
	int bet[MAX_ROUND], hand[MAX_ROUND], result[MAX_ROUND];
	int money, jetton;
} opp[MAX_PLAYER_NUM];

/* Opponent's Playing Style */
#define NA     0
#define NORMAL 1
#define TIGHT  2
#define LOOSE  3
#define BLUFF  4

/*
void testPrint() {
	int i;
	for (i = 0; i < MAX_PLAYER_NUM; i++) {
		printf("opp[%d]: %d, %d, %d\n", i+1, opp[i].pid, opp[i].bet[0], opp[i].money);
	}
}
*/

/* Analysis */
int updateData(int pid, int action, int num, int roundNum) {
	
}


int styleof(int pid) {

}


/* Application */
int estimate(int pid) { // estimating the most possible poker hand the opponent's got

}

#endif
