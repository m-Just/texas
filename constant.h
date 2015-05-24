/* This file defines all constant in common use */

#ifndef CONSTANT_H
#define CONSTANT_H

/* Actions */
#define CHECK 1
#define CALL  2
#define RAISE 3
#define ALLIN 4
#define FOLD  5
#define BLIND 6  // big and small blind bet.
#define SHOW  7  // showdown, including winning and losing, except folding.
#define QUIT  8  // gameover, when a player loses all his money.

/* Poker Hands */
#define HIGH_CARD 1
#define ONE_PAIR  2
#define TWO_PAIR  3
#define THREE_OF_A_KIND 4
#define STRAIGHT  5
#define FLUSH     6
#define FULL_HOUSE 7
#define FOUR_OF_A_KIND 8
#define STRAIGHT_FLUSH 9

/* Basic Information */
#define MAX_PLAYER_NUM 8
#define MAX_ROUND 500  // needs to be updated

#endif
