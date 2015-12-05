#include "queue.h"

struct node{
	CHESS_MOVE *move;
	struct node *next;
	struct node *previous;
	int checked;
};

struct queue{
	struct node *first;
	int size;
};

NODE *create_node(CHESS_MOVE *move){
	NODE *new_node = malloc(sizeof(NODE));
	if(new_node != NULL && move != NULL){
		new_node->move = move;
		new_node->next = new_node;
		new_node->previous = new_node;
		new_node->checked = 0;
	}else{
		if(new_node != NULL){
			free(new_node);
			new_node = NULL;
		}
	}

	return new_node;
}

int delete_node(NODE **node){
	if(node != NULL){
		if((*node) != NULL){
			delete_move( &((*node)->move) );
			free(*node);
			(*node) = NULL;
			return 0;
		}
		return 1;
	}
	return 2;
}

QUEUE *create_queue(void){
	QUEUE *queue = malloc(sizeof(QUEUE));
	if(queue != NULL){
		queue->first = NULL;
		queue->size = 0;
	}

	return queue;
}

int empty_queue(QUEUE *queue){
	if(queue != NULL && queue->size > 0) return 0;
	else return 1;
}

int enqueue(QUEUE *queue, CHESS_MOVE *move){
	NODE *node = create_node(move);
	if(queue != NULL){
		if(node != NULL){
			if(empty_queue(queue)) queue->first = node;
			else{
				node->previous = queue->first->previous;
				node->next = queue->first;
				node->previous->next = node;
				node->next->previous = node;
			}
			queue->size++;

			return 0;
		}
		return 1;
	}
	return 2;
}

int dequeue(QUEUE *queue){
	if(queue != NULL){
		if(!empty_queue(queue)){
			NODE *aux = queue->first;
			queue->first = (aux == queue->first->next ? NULL : queue->first->next);
			aux->next->previous = aux->previous;
			aux->previous->next = aux->next;
			delete_node(&aux);
			queue->size--;

			return 0;
		}
		return 1;
	}
	return 2;
}

NODE *front_queue(QUEUE *queue){
	if(!empty_queue(queue)) return queue->first;
	return NULL;
}

int delete_queue(QUEUE **queue){
	if(queue != NULL){
		if((*queue) != NULL){
			while(!empty_queue(*queue)) dequeue(*queue);
			free(*queue);
			(*queue) = NULL;

			return 0;
		}
		return 1;
	}
	return 2;
}

void print_queue(QUEUE *queue){
	if(!empty_queue(queue)){
		NODE *aux = queue->first;

		do{
			print_move(aux->move);
			aux = aux->next;
		}while(aux != queue->first);
	}
}

void check_repeat_moves(QUEUE *queue){
	
}
