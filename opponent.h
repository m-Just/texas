#ifndef OPPONENT_H
#define OPPONENT_H
#include"constant.h"

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

struct ANAOPP{           // database structure, for the analysis of the opponents
	int pid, style;
	int bet[MAX_ROUND][5], hand[MAX_ROUND], pattern[MAX_ROUND];
	int maxbet[10]; // nine different poker hands. [0] for overall.
	int currentAction, currentStage, money, jetton;
	double avrgBet, variance, foldrate; // these values will be useful only after gathering enough info
};


int rnd(double val);

/* Analysis */
int hash(int id) ;

void iterate(double* value, double change, int roundNum) ;

int updateData(int id, int action, int num, int jet, int m, int stage, int roundNum) ;

int styleAnalyse(int id) ;

int patternAnalyse(int id) ;

double jettonPara(int id, int stage, int roundNum) ;

int estHand(int id, int* card, int cardNum, int stage, int roundNum) ;

// note: estimation is not available before the stage of FLOP
int estFold(int id, int* card, int cardNum, int stage, int roundNum) ;


#endif
