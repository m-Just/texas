#include "opponent.h"
#include <stdio.h>
#include <math.h>
#include "constant.h"
#include "conversion.h"

ANAOPP opp[MAX_PLAYER_NUM];

int rnd(double val) {
	return (int)(val + 0.5);
}

/* Analysis */
int hash(int id) {
	int i = 0;
	while (i < MAX_PLAYER_NUM && opp[i++].pid != 0) if (opp[i].pid == id) break;
	opp[i].pid = id;
	return i;
}

void iterate(double* value, double change, int roundNum) {
	*value = (*value * roundNum + change) / (double)(roundNum+1);
}

double coco(void* x, void* y, int stepsize) { // Correlation Coefficient
	
}

// update after every inquire, notify(if available), showdown and pot-win message
// for the representation and detailed definition of the actions and stage, please view "constant.h"
int updateData(int id, int action, int num, int jet, int m, int stage, int roundNum) { // 0 <= roundNum <= MAX_ROUND-1
	// num is only available for CALL, RAISE, BLIND, SHOW and POT actions.
	// ESPECIALLY, input the poker hands into "num" for SHOW action.
	// otherwise, input 0 if not available.	
	int i = hash(id); int j;
	opp[i].currentAction = action;					    
	opp[i].currentStage  = stage;
	if (m != -1)   opp[i].money[roundNum]  = m;     /* notice: the value of "m" and "jet" is the last value received from the server */
	if (jet != -1) opp[i].jetton[roundNum] = jet;   /* when the action is POT, the change in "m" and "jet" is done by "num" */

	if 	(action == QUIT || action == CHECK) {/* do nothing */}
	else if (action == CALL || action == RAISE) opp[i].bet[roundNum][stage-1] = num;
	else if (action == ALLIN) opp[i].bet[roundNum][stage-1] = num;
	else if (action == FOLD)  opp[i].hand[roundNum] = NA;	
	else if (action == BLIND) opp[i].bet[roundNum][stage-1] = num;
	else if (action == SHOW) {
		opp[i].hand[roundNum] = num; // num is the code for poker hands
		if (num < 2 && opp[i].bet[roundNum][stage-1] > (double)opp[i].jetton[roundNum]/3) opp[i].style = BLUFF;
		for (j = 1; j <= STRAIGHT_FLUSH-num; j++)
			if (opp[i].maxbet[num+j] && opp[i].bet[roundNum][stage-1] > opp[i].maxbet[num+j]) {
				opp[i].style = BLUFF; break;
			}
		if (opp[i].style != BLUFF)
			if (opp[i].maxbet[num] < opp[i].bet[roundNum][stage-1]) {
				opp[i].maxbet[num] = opp[i].bet[roundNum][stage-1];
				opp[i].maxbetRound[num] = roundNum;
			}
	}
	else if (action == POT)  {
		opp[i].jetton[roundNum+1] = opp[i].jetton[roundNum]+num; 
		opp[i].money[roundNum+1]  = opp[i].money[roundNum]+num;
	}
	else printf("Error: Invalid action#%d by player#%d at round#%d!\n", action, id, roundNum+1);
	
	opp[i].maxbet[0] = opp[i].maxbet[0] < opp[i].bet[roundNum][stage-1] ? opp[i].bet[roundNum][stage-1] : opp[i].maxbet[0];

	if (action == FOLD || action == SHOW) {  // the errors of the values below are big when roundNum is small
		iterate(&opp[i].avrgBet,  (double)opp[i].bet[roundNum][stage-1], roundNum);
		iterate(&opp[i].variance, pow((double)(opp[i].bet[roundNum][stage-1])-opp[i].avrgBet, 2.0), roundNum);
		iterate(&opp[i].foldrate, (double)(action%SHOW)/FOLD, roundNum);
	}
	
	if (roundNum > 0 && !roundNum%10 && opp[i].style != BLUFF) {
		if 	(opp[i].avrgBet < 2*BIG_BLIND) opp[i].style = TIGHT;
		else if (opp[i].avrgBet < 5*BIG_BLIND) opp[i].style = NORMAL;
		else    /* avrgBet >= 5BB */	       opp[i].style = LOOSE;
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

double jettonPara(int id, int stage, int roundNum, int maxbetRound) {
	int endStage, i = hash(id);
	for (endStage = 1; endStage < POT_WIN; endStage++) if (!opp[i].bet[roundNum][endStage]) break;
	return (double)(opp[i].bet[roundNum][stage-1]+opp[i].jetton[roundNum])/
	       (double)(opp[i].bet[maxbetRound][endStage-1]+opp[i].jetton[maxbetRound]);
}

/* Playing Style Decisive Critrion */
// bet limits of different hands and foldrate(for estimating hands and force folding)
// bet patterns (all_in? patient bet? lure bet?)
// strategy concerning jetton and money
// be aware of changing style: short-term analysis also needed other than the long-term analysis
// re-analysis when making mistake in estimating


/* Application */
// notice: update data before any application

// the "card" and "cardNum" is only the values of public cards
int estHand(int id, int* card, int cardNum, int stage, int roundNum) { // estimating the most possible poker hand the opponent's got
	// by study the opponent's possible hands and pattern of actions(style).
	int i = hash(id);

	int point[13] = {0}, color[4] = {0};
	int m, n;
	for (m = 0; m < cardNum; m++) {
		point[pointof(card[m])-1]++;
		color[colorof(card[m])]++;
	}
	
	int lowestHand = 0, potentHand = 0, highestHand = 0;
	
	int pmax = 0, pair = 0, cmax = 0, smax = 0, c;
	for (m = 0; m < 4;  m++) { if (cmax < color[m]) { cmax = color[m]; c = m; } }
	for (m = 0; m < 13; m++) { pmax = pmax < point[m] ? point[m] : pmax; if (point[m] == 2) pair++; }
	for (m = 0; m < 9;  m++) { 
		int cnt = 0;
		for (n = 0; n < 5; n++) if (point[m+n]) cnt++;
		smax = smax < cnt ? cnt : smax;
	}

	int tmp = THREE_OF_A_KIND + (pair-1)*(FULL_HOUSE-THREE_OF_A_KIND);  
	if 	(pmax == 1) { lowestHand = HIGH_CARD; 	    potentHand = ONE_PAIR;       highestHand = THREE_OF_A_KIND;}
	else if (pmax == 2) { lowestHand = pair+1; 	    potentHand = tmp;            highestHand = FOUR_OF_A_KIND; }
	else if (pmax == 3) { lowestHand = THREE_OF_A_KIND; potentHand = FULL_HOUSE;     highestHand = FOUR_OF_A_KIND; }
	else if (pmax == 4) { lowestHand = FOUR_OF_A_KIND;  potentHand = FOUR_OF_A_KIND; highestHand = FOUR_OF_A_KIND; }
	if	(smax >= 3) { if (pair != 2 && pmax < 3) potentHand = STRAIGHT; }
	if 	(cmax >= 3) { potentHand = potentHand > FLUSH ? potentHand : FLUSH; }

	if (smax >= 3 && cmax >=3) {  // check for STRAIGHT_FLUSH
		int cnt = 0, p[5] = {0};
		for (m = 0; m < cardNum; m++) if (colorof(card[m]) == c) p[cnt++] = pointof(card[m]);
		for (m = 0; m < cnt-1; m++) for (n = m+1; n < cnt; n++) if (p[n] < p[m]) p[m]^=p[n]^=p[m]^=p[n];
		for (m = 0; m < cnt-2; m++) if (p[m+2]-p[m] < 5) highestHand = STRAIGHT_FLUSH;
		for (m = 0; m < cnt-3; m++) if (p[m+3]-p[m] < 5) potentHand  = STRAIGHT_FLUSH;
	}

	/* main logic */  // the logic concerning decision making and bluff-playing needs to be improved
	if (opp[i].currentAction == CHECK || opp[i].currentAction == FOLD) { 
		if (opp[i].maxbet[lowestHand] < opp[i].bet[roundNum][stage-1]) {
			opp[i].maxbet[lowestHand] = opp[i].bet[roundNum][stage-1];
			opp[i].maxbetRound[lowestHand] = roundNum;
		}
		return lowestHand;
	} else
	if (opp[i].currentAction == RAISE || opp[i].currentAction == CALL) {
		if (opp[i].maxbet[potentHand]) {
			if (opp[i].bet[roundNum][stage-1] >
				jettonPara(id, stage, roundNum, opp[i].maxbetRound[potentHand])*(highestHand-potentHand+1)*opp[i].maxbet[potentHand] ||
			   (opp[i].maxbet[highestHand-1] &&
			    opp[i].bet[roundNum][stage-1] >
				jettonPara(id, stage, roundNum, opp[i].maxbetRound[highestHand-1])*opp[i].maxbet[highestHand-1])) {
				return highestHand;
			} else  return potentHand;
		} else return UNKNOWN;
	} else
	if (opp[i].currentAction == ALLIN) {
		if (opp[i].style == BLUFF) return potentHand-(potentHand != HIGH_CARD);
		else 			   return potentHand+(potentHand != STRAIGHT_FLUSH);
	} else return UNKNOWN;
	
}		

// note: estimation is not available before the stage of FLOP
int estFold(int id, int* card, int cardNum, int stage, int roundNum) { // return the estimated amount of bet that would force the opponent's to fold
	int i = hash(id);
	int eHand = estHand(id, card, cardNum, stage, roundNum);       // the return value
	if (opp[i].foldrate < 0.2 && opp[i].style == LOOSE) return 0;
	if (opp[i].foldrate > 0.8 && opp[i].style == TIGHT) return opp[i].avrgBet*pow(2, eHand-1);
	if (stage > PREFLOP && eHand > UNKNOWN && opp[i].maxbet[eHand])
		return rnd(jettonPara(id, stage, roundNum, opp[i].maxbetRound[eHand])*opp[i].maxbet[eHand]/opp[i].foldrate);
	else 	return 0;  // return 0 for unavailability
}
