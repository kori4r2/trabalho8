#include "queue.h"

struct node{
	CHESS_MOVE *move;
	struct node *next;
	struct node *previous;
	unsigned char checked;
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

// Funcao que checa repeticoes na fila de start ate o ultimo nó
void check_repeats(QUEUE *queue, NODE *start){
	if(start == NULL) return;

	NODE *last = queue->first->previous;
	// Se houver apenas um item, simplesmente indica que ele ja foi checado
	if(start == last) start->checked = 1;
	else{
		NODE **coincidences;
		NODE *node;
		int counter, i, j;
		unsigned char type;
		unsigned char *results;
		// Enquanto o ultimo item sendo avaliado não for o primeiro, nem todos foram checados
		while(last != start){
			// Se o item ainda não foi checado
			if(!last->checked){
				node = last->previous;
				coincidences = NULL;
				results = NULL;
				counter = 0;
				// Avalia todos os nós anteriores
				do{
					// Caso o nó ainda não tenha sido checado
					if(!node->checked){
						// Se os dois movimentos tiverem o mesmo destino
						if(compare_moves(node->move, last->move) > 0){
							// Marca o nó como checado
							node->checked = 1;
							// Armazena o nó e o resultado dessa comparacao
							counter++;
							results = (unsigned char*)realloc(results, sizeof(unsigned char)*counter);
							coincidences = (NODE**)realloc(coincidences, sizeof(NODE*)*counter);
							results[counter-1] = compare_moves(node->move, last->move);
							coincidences[counter-1] = node;
						}
					}
					node = node->previous;
				// Para apos checar o nó inicial
				}while(node != start);
				// Caso tenham sido encontradas coincidencias de destino
				if(counter > 0){
					// Percorre todas as coincidencias
					for(i = 0; i < counter; i++){
						// Checa todas entre si, mas evitando checar mais de uma vez um mesmo par
						node = coincidences[i];
						for(j = i+1; j < counter; j++){
							// Caso seja encontrado um tipo diferente de conflito, altera o resultado desta coincidencia
							if(j != i && (type = compare_moves(node->move, coincidences[j]->move)) != results[i]){
								results[i] = type;
								j = counter;
							}
						}
					}
					// Armazena o primeiro tipo de confilto
					type = results[0];
					// Percorre todos os demais conflitos
					for(i = 1; i < counter; i++){
						// Caso haja algum conflito diferente, muda o tipo para 3
						if(results[i] != type){
							type = 3;
							i = counter;
						}
					}
					// altera o tipo de repeticao de todos os nos analisados para o resultado da checagem
					set_repeat(last->move, type);
					for(i = 0; i < counter; i++){
						set_repeat(coincidences[i]->move, type);
					}
					free(coincidences);
					free(results);
					results = NULL;
					coincidences = NULL;
					counter = 0;
				}
				// Marca o nó como checado
				last->checked = 1;
			}
			last = last->previous;
		}
	}
}

CHESS_MOVE *front_queue(QUEUE *queue){
	if(!empty_queue(queue)) return queue->first->move;
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
