#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>
#include <stdio.h>
#include "chess_move.h"

typedef struct node NODE;
typedef struct queue{
	struct node *first;
	int size;
}QUEUE;

NODE *create_node(CHESS_MOVE*);
int delete_node(NODE**);
QUEUE *create_queue(void);
int empty_queue(QUEUE*);
int enqueue(QUEUE*, CHESS_MOVE*);
int dequeue(QUEUE*);
void check_repeats(QUEUE*, NODE*);
CHESS_MOVE *front_queue(QUEUE*);
int delete_queue(QUEUE**);
void print_queue(QUEUE*);

#endif
