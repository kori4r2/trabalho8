#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>
#include <stdio.h>
#include "chess_move.h"

typedef struct node NODE;
typedef struct queue QUEUE;

NODE *create_node(CHESS_MOVE*);
int delete_node(NODE**);
QUEUE *create_queue(void);
int empty_queue(QUEUE*);
int enqueue(QUEUE*, CHESS_MOVE*);
int dequeue(QUEUE*);
NODE *front_queue(QUEUE*);
int delete_queue(QUEUE**);
void print_queue(QUEUE*);

#endif
