#ifndef _CHESS_H_
#define _CHESS_H_

#define WHITES_SIDE 2
#define BLACKS_SIDE 1
#define DELIMITERS " /"
#define W_ROOK 'R'
#define W_KNIGHT 'N'
#define W_BISHOP 'B'
#define W_QUEEN 'Q'
#define W_KING 'K'
#define W_PAWN 'P'
#define B_ROOK 'r'
#define B_KNIGHT 'n'
#define B_BISHOP 'b'
#define B_QUEEN 'q'
#define B_KING 'k'
#define B_PAWN 'p'
#define WHITES_TURN 'w'
#define BLACKS_TURN 'b'

#include "chess_move.h"
#include "queue.h"
#include "my_strings.h"

typedef struct piece PIECE;
typedef struct list PIECE_LIST;
typedef struct table TABLE;

PIECE_LIST *create_piece_list(TABLE*);
int delete_list(PIECE_LIST**);
// temporary
int print_list(PIECE_LIST*);
void list_moves(TABLE*, QUEUE*, PIECE_LIST*);
//
TABLE *create_table(void);
int read_table(FILE*, TABLE*);
int move_piece(char*, TABLE*);
char* get_fen(TABLE*);
char *dupe_move(TABLE*);
int is_check(TABLE*);
int delete_table(TABLE**);

int print_table(TABLE*);

#endif
