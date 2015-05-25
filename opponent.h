#ifndef OPPONENT_H
#define OPPONENT_H

#include <stdio.h>
#include <math.h>
#include "constant.h"
#include "conversion.h"

// the styles and patterns may not be needed.
/* Opponent's Playing Styles */
#define NA     0
#define NORMAL 1
#define TIGHT  2
#define LOOSE  3
#define BLUFF  4

/* Opponent's Bet Patterns */
#define NA	0
#define PATIENT 1
#define LURE    2
#define PASSIVE 3
#define CHAOS   4

struct {           // database structure, for the analysis of the opponents
	int pid, style;
	int bet[MAX_ROUND], hand[MAX_ROUND], pattern[MAX_ROUND];
	int maxbet[10]; // nine different poker hands. [0] for overall.
	int currentAction, money, jetton;
	double avrgBet, variance, foldrate; // these values will be useful only after gathering enough info
} opp[MAX_PLAYER_NUM];


/* Analysis */
int hash(int id) {
	int i = 0;
	while (i < MAX_PLAYER_NUM && opp[i++].pid != 0)
		if (opp[i].pid == id) break;
	opp[i].pid = id;
	return i;
}

void iterate_c(double* value, double change, int roundNum) {
	*value = (*value * roundNum + change) / (double)(roundNum+1);
}

// update after every inquire, notify(if available), showdown and pot-win message
// for the representation and detailed definition of the actions, please view "constant.h"
int updateData(int id, int action, int num, int jet, int m, int roundNum) { // num is only available for CALL, RAISE, BLIND, SHOW and POT actions.
	int i = hash(id);					    	    // ESPECIALLY, input the poker hands for SHOW action.
	opp[i].currentAction = action;					    // otherwise, input 0 if not available.	
	opp[i].money = m;      /* notice: the value of "m" and "jet" is the last value received from the server */
	opp[i].jetton = jet;   /* when the action is POT, the change in "m" and "jet" is done by "num" */

	if 	(action == QUIT || action == CHECK) {/* do nothing */}
	else if (action == CALL || action == RAISE) opp[i].bet[roundNum] = num;
	else if (action == ALLIN) opp[i].bet[roundNum] 	= num;
	else if (action == FOLD)  opp[i].hand[roundNum] = NA;	
	else if (action == BLIND) opp[i].bet[roundNum] 	= num;
	else if (action == SHOW) {
		opp[i].hand[roundNum] = num; // poker hands
		opp[i].maxbet[num] < opp[i].bet[roundNum] ? opp[i].bet[roundNum] : opp[i].maxbet[num];
	}
	else if (action == POT)  {opp[i].jetton += num; opp[i].money += num;}
	else printf("Error: Invalid action#%d by player#%d at round#%d!\n", action, id, roundNum+1);
	
	opp[i].maxbet[0] < opp[i].bet[roundNum] ? opp[i].bet[roundNum] : opp[i].maxbet[0];

	if (action == FOLD || action == SHOW) {  // the errors of the values below are big when roundNum is small
		iterate_c(&opp[i].avrgBet,  (double)opp[i].bet[roundNum], roundNum);
		iterate_c(&opp[i].variance, pow(opp[i].bet[roundNum]-opp[i].avrgBet, 2), roundNum);
		iterate_c(&opp[i].foldrate, (double)(action%SHOW)/FOLD, roundNum);
	}
	
	// check if the estimate() is right. if not, re-analyse and considering bluff-playing.
}

int styleAnalyse(int id) {
	int i = hash(id);
	// avrgBet, maxbet, foldrate
}

int patternAnalyse(int id) {
	int i = hash(id);
}

/* Playing Style Decisive Critrion */
// bet limits of different hands and foldrate(for estimating hands and force folding)
// bet patterns (all_in? patient bet? lure bet?)
// strategy concerning jetton and money
// be aware of changing style: short-term analysis also needed other than the long-term analysis
// re-analysis when making mistake in estimating


/* Application */
// notice: update data before any application

int estHand(int id, int* card, int cardNum, int roundNum) { // estimating the most possible poker hand the opponent's got
	// by study the opponent's possible hands and pattern of actions(style).
	int i = hash(id);

	int point[14] = {0}, color[4] = {0};
	int m;
	for (m = 0; m < cardNum; m++) {
		point[pointof(card[m])]++;
		color[colorof(card[m])]++;
	}
	
	int lowestHand = 0, potentHand = 0, highestHand = 0;
	
	int pmax = 0, pair = 0, cmax = 0;
	for (m = 0; m < 4;  m++)   cmax < color[m] ? color[m] : cmax;
	for (m = 0; m < 14; m++) { pmax < point[m] ? point[m] : pmax; if (point[m] == 2) pair++; }

	int tmp = THREE_OF_A_KIND + (pair-1)*(FULL_HOUSE-THREE_OF_A_KIND);  
	if 	(pmax == 1) { lowestHand = HIGH_CARD; 	    potentHand = ONE_PAIR;       highestHand = THREE_OF_A_KIND;}
	else if (pmax == 2) { lowestHand = pair+1; 	    potentHand = tmp;            highestHand = FOUR_OF_A_KIND; }
	else if (pmax == 3) { lowestHand = THREE_OF_A_KIND; potentHand = FULL_HOUSE;     highestHand = FOUR_OF_A_KIND; }
	else if (pmax == 4) { lowestHand = FOUR_OF_A_KIND;  potentHand = FOUR_OF_A_KIND; highestHand = FOUR_OF_A_KIND; }
	if (cmax >= 3) { potentHand > FLUSH ? potentHand : FLUSH; highestHand = STRAIGHT_FLUSH; }

	// unfinished...
	// study the bet by the opponent and compare with the lowest, potent and highest hands.
	// how to study? compare with style and pattern
}		


int estFold(int id, int* card, int roundNum) { // return the estimated amount of bet that would force the opponent's to fold

}


#endif
