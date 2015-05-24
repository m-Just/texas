#ifndef OPPONENT_H
#define OPPONENT_H

#include <stdio.h>
#include "constant.h"

struct {           // database structure, for the analysis of the opponents
	int pid;
	int bet[MAX_ROUND], hand[MAX_ROUND];
	int money, jetton;
	int currentAction;
	int style;
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
int hash(int id) {
	int i = 0;
	
	while (i < MAX_PLAYER_NUM && opp[i++].pid != 0)
		if (opp[i].pid == id) break;
	opp[i].pid = id;

	return i;
}

// update after every inquire, notify(if available), showdown and pot-win message
// for the representation and detailed definition of the actions, please view "constant.h"
int updateData(int id, int action, int num, int jet, int m, int roundNum) { // num is only available for CALL, RAISE, BLIND and SHOW actions.
	int i = hash(id);					    	    // ESPECIALLY, input the poker hands for SHOW action.
	opp[i].currentAction = action;					    // otherwise, input 0 if not available.	

	if 	(action == QUIT || action == CHECK) {/* do nothing */}
	else if (action == CALL || action == RAISE) opp[i].bet[roundNum] = num;
	else if (action == ALLIN) opp[i].bet[roundNum] 	= num;
	else if (action == FOLD)  opp[i].hand[roundNum] = NA;	
	else if (action == BLIND) opp[i].bet[roundNum] 	= num;
	else if (action == SHOW)  opp[i].hand[roundNum] = num; // poker hands
	else printf("Error: Invalid action#%d by player#%d at round#%d!\n", action, id, roundNum+1);

	opp[i].money = m;
	opp[i].jetton = jet;

	// check if the estimate() is right. if not, re-analyse the style and considering bluff-playing.
}


int styleAnalyse(int id) {
	int i = hash(id);
}

/* Playing Style Confirmation Critrion */
// bet limits of different hands (for estimating hands and force folding)
// bet patterns (all_in? patient bet? lure bet?)
// strategy concerning jetton and money
// be aware of changing style: short-term analysis also needed other than the long-term analysis
// re-analysis when making mistake in estimating

/* Application */
int estimate(int id) { // estimating the most possible poker hand the opponent's got
	// by study the opponent's pattern of actions(style).
	int i = hash(id);
}

#endif
