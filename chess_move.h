#ifndef _CHESS_MOVE_H_
#define _CHESS_MOVE_H_

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

typedef struct move CHESS_MOVE;

CHESS_MOVE *create_move(char, int, char, int, char, unsigned char, char);
int compare_moves(CHESS_MOVE*, CHESS_MOVE*);
int swap_moves(CHESS_MOVE**, CHESS_MOVE**);
int delete_move(CHESS_MOVE**);
void print_move(CHESS_MOVE*);

#endif
