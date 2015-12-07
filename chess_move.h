#ifndef _CHESS_MOVE_H_
#define _CHESS_MOVE_H_

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

typedef struct move{
	char piece;
	int origin_rank;
	int origin_file;
	int destiny_rank;
	int destiny_file;
	unsigned char capture;
	unsigned char repeat;
	char special;
}CHESS_MOVE;

CHESS_MOVE *copy_move(CHESS_MOVE*);
CHESS_MOVE *create_move(char, int, char, int, char, unsigned char, char);
void set_repeat(CHESS_MOVE*, unsigned char);
int compare_moves(CHESS_MOVE*, CHESS_MOVE*);
int swap_moves(CHESS_MOVE**, CHESS_MOVE**);
int delete_move(CHESS_MOVE**);
void print_move(CHESS_MOVE*);

#endif
