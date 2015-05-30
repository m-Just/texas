#include "opponent.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "constant.h"
#include "conversion.h"
#include <string.h>

ANAOPP opp[MAX_PLAYER_NUM];

void oppclear()
{
	memset(opp, 0, sizeof(opp));
}

int rnd(double val) {
	return (int)(val + 0.5);
}

/* Analysis */
int hash(int id) {
	int i;
	for (i = 0; i < MAX_PLAYER_NUM && opp[i].pid != 0; i++) if (opp[i].pid == id) break;
	opp[i].pid = id;
	return i;
}

void iterate(double* value, double change, int roundNum) {
	*value = (*value * roundNum + change) / (double)(roundNum+1);
}

double coco(int* x, int* y, int ini_i, int steplen, int stepsize, int totallen) { // Correlation Coefficient (-1~1)
	int i, j, cnt = 0, yistmp = 0;
	if (totallen < steplen + stepsize) return 1;
	if (y == NULL) {
		y = (int*)malloc(sizeof(int)*totallen); yistmp = 1;
		for (i = ini_i; i < ini_i+totallen; i++) if (x[i]) y[i] = i;
	}
	int len = (totallen-steplen)/stepsize+1;
	double xa[len], ya[len];
	double xaa = 0, yaa = 0;

	cnt = 0;
	for (i = ini_i; i < ini_i+(len-1)*stepsize; i+=stepsize) {
		xa[cnt] = 0; ya[cnt] = 0;
		for (j = i; j < i+steplen; j++) { if (x[j]) xa[cnt] += x[j]; ya[cnt] += y[j]; } cnt++;
	}
	for (i = 0; i < len; i++) { xaa += xa[i]; yaa += ya[i]; }
	xaa /= len; yaa /= len;

	if (yistmp) free(y);

	double upper = 0, xd = 0, yd = 0;
	for (i = 0; i < len; i++) {
		upper += (xa[i]-xaa)*(ya[i]-yaa);
		xd    += pow(xa[i]-xaa, 2.0);
		yd    += pow(ya[i]-yaa, 2.0);
	}

	if (xd && yd) 
		return upper/sqrt(xd*yd);
	else	return 1;    // return 1 for absolute linear correlation
}

// update after every inquire, notify(if available), showdown and pot-win message
// for the representation and detailed definition of the actions and stage, please view "constant.h"
int updateData(int id, int action, int num, int jet, int m, int stage, int roundNum) { // 0 <= roundNum <= MAX_ROUND-1
	// all the actions except SHOW need to input the total bet into "num"
	// ESPECIALLY, input the poker hands into "num" for SHOW action.
	// otherwise, input 0 if not available.	
#ifdef TEST
	FILE *fout = fopen("update.txt", "a");
	//fprintf(fout, "id: %7d action: %7d num: %7d stage: %7d roundNum: %7d\n", id, action, num, stage, roundNum);
#endif
	int i = hash(id); int j;
	opp[i].currentAction = action;
	opp[i].currentStage  = stage;
	if (m 	!= -1) opp[i].money[roundNum]  = m;     /* notice: the value of "m" and "jet" is the last value received from the server */
	if (jet != -1) opp[i].jetton[roundNum] = jet;
	int* b = &opp[i].bet[roundNum][stage-1];

	if 	(action == QUIT || action == CHECK) *b = num;
	else if (action == CALL || action == RAISE) *b = num;
	else if (action == ALLIN) *b = num;
	else if (action == FOLD)  { *b = num; opp[i].hand[roundNum] = NA; }
	else if (action == BLIND) *b = num;
	else if (action == SHOW) {
		opp[i].hand[roundNum] = num%10; // num := best_hand*10+nut_hand
		/* bluff detection */
		if (roundNum >= bluff_detection_start_roundNum && opp[i].foldrate != 0)
			if (opp[i].hand[roundNum]+2 <= num/10 && opp[i].bet[roundNum][RIVER-1] > opp[i].avrgBet/opp[i].foldrate)
				opp[i].style = BLUFF;
		for (j = 1; j <= STRAIGHT_FLUSH-opp[i].hand[roundNum]; j++)
			if (opp[i].maxbet[opp[i].hand[roundNum]+j] && opp[i].bet[roundNum][RIVER-1] >
			    jettonPara(id, stage, roundNum, opp[i].maxbetRound[opp[i].hand[roundNum]+j])*opp[i].maxbet[opp[i].hand[roundNum]+j]) {
				opp[i].style = BLUFF; break;
			}

		if (opp[i].style != BLUFF)
			if (opp[i].maxbet[num] < opp[i].bet[roundNum][RIVER-1]) {
				opp[i].maxbet[num] = opp[i].bet[roundNum][RIVER-1];
				opp[i].maxbetRound[num] = roundNum;
			}
	}
	else printf("Error: Invalid action#%d by player#%d at round#%d!\n", action, id, roundNum+1);
	
	if (opp[i].maxbet[0] < *b) { opp[i].maxbet[0] = *b; opp[i].maxbetRound[0] = roundNum; }

	if (action == FOLD || action == SHOW) opp[i].lastbet[roundNum] = *b;

	if (roundNum > 0 && !(roundNum%10) && opp[i].style != BLUFF) {
		if 	(opp[i].avrgBet < 2*BIG_BLIND) opp[i].style = TIGHT;
		else if (opp[i].avrgBet < 5*BIG_BLIND) opp[i].style = NORMAL;
		else    /* avrgBet >= 5BB */	       opp[i].style = LOOSE;
	}

#ifdef TEST
	fclose(fout);
#endif
	// check if the estimate() is right. if not, re-analyse and considering bluff-playing.
}

void compute(int roundNum)
{
#ifdef TEST
	FILE *fout = fopen("compute.txt", "a");
#endif
	for (int i = 0; i < 8; i++) if (opp[i].money > 0)
	{
		int k = RIVER - 1;
		while (k > 0 && opp[i].bet[roundNum][k] == 0) k--;
		double lastbet = opp[i].bet[roundNum][k];
		opp[i].lastbet[roundNum] = (int)lastbet;
		iterate(&opp[i].avrgBet, lastbet, roundNum);
		iterate(&opp[i].variance, pow(lastbet - opp[i].avrgBet, 2.0), roundNum);
		//fprintf(fout, "round: %d id: %d lastbet: %d average: %lf variance: %lf\n", roundNum, opp[i].pid, (int)lastbet, opp[i].avrgBet, opp[i].variance);
#ifdef TEST		
		iterate(&opp[i].foldrate, (double)(opp[i].currentAction==FOLD), roundNum);
#endif
	}
#ifdef TEST
	fclose(fout);
#endif
}

int styleAnalyse(int id) {
	int i = hash(id);
	// avrgBet, maxbet, foldrate
}

int patternAnalyse(int id) {
	int i = hash(id);
}

double jettonPara(int id, int stage, int roundNum, int maxbetRnd) {
	int endStage, i = hash(id);
	for (endStage = 1; endStage < RIVER; endStage++) if (!opp[i].bet[maxbetRnd][endStage]) break;
	double tmp = (double)(opp[i].bet[roundNum][stage-1]+opp[i].jetton[roundNum])/
	       	     (double)(opp[i].bet[maxbetRnd][endStage-1]+opp[i].jetton[maxbetRnd]);
	return 1.0+(tmp-1.0)*opp[i].cc;
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
#ifdef TEST
	FILE *fout = fopen("estHand.txt", "a");
#endif
	int i = hash(id);
	int b = opp[i].bet[roundNum][stage-1];

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
	
#ifdef TEST
		fprintf(fout, "round %d player%d : lowest:%d potent:%d highest:%d\n", roundNum, opp[i].pid, lowestHand, potentHand, highestHand);
#endif
	double betcchand = 0; int cnt = 0;
	for (m = 1; m < 10; m++) if (opp[i].maxbet[m]) cnt++;
	if (cnt > 2) betcchand = coco(opp[i].maxbet, NULL, 1, 1, 1, 9);
#ifdef TEST
	if (fabs(betcchand)>1) { fprintf(fout, "Invalid value of betcchand: %lf, setting to default value 0.\n", betcchand); betcchand = 0; }
#endif
	/* main logic */  // the logic concerning decision making and bluff-playing needs to be improved
	if (opp[i].currentAction == CHECK || opp[i].currentAction == FOLD) { 
		if (opp[i].maxbet[lowestHand] < opp[i].bet[roundNum][stage-1]) {
			opp[i].maxbet[lowestHand] = b; 
			opp[i].maxbetRound[lowestHand] = roundNum;
		}
		return lowestHand;
	} else
	if (opp[i].currentAction == RAISE || opp[i].currentAction == CALL)  {
		if (opp[i].maxbet[potentHand]) {
			if (betcchand > 0.6)
				for (m = potentHand; m <= highestHand; m++)
					if (b > /*slope*/1.5*(m-potentHand+1)*opp[i].maxbet[potentHand])   // the slope should be further determined
						return m+(m != STRAIGHT_FLUSH);
			if (opp[i].maxbet[highestHand-1] &&
			    b > jettonPara(id, stage, roundNum, opp[i].maxbetRound[highestHand-1])*opp[i].maxbet[highestHand-1]) {
				return highestHand;
			} else  return potentHand;    // not accurate enough
		} else return UNKNOWN;
	} else
	if (opp[i].currentAction == ALLIN) {
		if (opp[i].style == BLUFF) return potentHand;
		else 			   return potentHand+(potentHand != STRAIGHT_FLUSH);
	} else return UNKNOWN;
#ifdef TEST
	fclose(fout);
#endif
}		

// note: estimation is not available before the stage of FLOP
int estFold(int id, int* card, int cardNum, int stage, int roundNum) { // return the estimated amount of bet that would force the opponent's to fold
#ifdef TEST
	FILE* fout = fopen("estFold.txt", "a");
#endif
	int i = hash(id);
	int eHand = estHand(id, card, cardNum, stage, roundNum);       // the return value
#ifdef TEST
	fprintf(fout, "Estimated hand#%d of player#%d at stage#%d in round#%d\n", eHand, id, stage, roundNum);
#endif
	if (opp[i].foldrate < 0.2 && opp[i].style == LOOSE) return -1;
	if (opp[i].foldrate > 0.8 && opp[i].style == TIGHT) return opp[i].avrgBet*pow(eHand+1, 2);
	if (stage > PREFLOP && stage <= RIVER && eHand > UNKNOWN && opp[i].maxbet[eHand])
		return rnd(jettonPara(id, stage, roundNum, opp[i].maxbetRound[eHand])*opp[i].maxbet[eHand]/opp[i].foldrate);
	else 	return 0;
#ifdef TEST
	fclose(fout);
#endif
}
