/* This file defines all constant in common use */

#ifndef CONSTANT_H
#define CONSTANT_H

/* Actions */
#define CHECK 1
#define CALL  2
#define RAISE 3
#define ALLIN 4
#define FOLD  5
#define BLIND 6  // both big and small blind bet.
#define SHOW  7  // showdown and pot distribution, including both winning and losing occasions.
#define POT SHOW
#define QUIT  8  // gameover, when a player loses all his money.

/* Poker Hands */
#define UNKNOWN   0
#define HIGH_CARD 1
#define ONE_PAIR  2
#define TWO_PAIR  3
#define THREE_OF_A_KIND 4
#define STRAIGHT  5
#define FLUSH     6
#define FULL_HOUSE 7
#define FOUR_OF_A_KIND 8
#define STRAIGHT_FLUSH 9

/* Get Msg */
#define SEAT_MSG 1
#define GAME_OVER_MSG 2
#define BLIND_MSG 3
#define HOLD_MSG 4
#define INQUIRE_MSG 5
#define COM_CARDS_MSG 6
#define SHOW_MSG 7
#define POT_MSG 8
#define NOTIFY_MSG 9

/* Game Stage */
#define PREFLOP 1 // including blind
#define FLOP    2
#define TURN    3
#define RIVER   4 // including pot_win
#define POT_WIN 5

/* Basic Information */  // updated 5.25
#define MAX_PLAYER_NUM 8
#define MAX_ROUND 600
#define START_MONEY  4000
#define START_JETTON 2000
#define BIG_BLIND   40
#define SMALL_BLIND 20

/*Macro for test*/
#define TEST
#define TESTNEW
#ifdef TEST
#define WRITE_IN_FILE
#endif
#define max(a, b) ((a) < (b))?(b):(a)

/*Abbr*/
#define SGI socket_get_int(fd)
#define IOW get_word(msg, fd) //ignore one word from socket
#endif
