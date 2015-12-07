#include "chess_move.h"

struct move{
	char piece;
	int origin_rank;
	int origin_file;
	int destiny_rank;
	int destiny_file;
	unsigned char capture;
	unsigned char repeat;
	char special;
};

void set_repeat(CHESS_MOVE *move, unsigned char repeat){
	if(move != NULL) move->repeat = repeat;
}

CHESS_MOVE *create_move(char piece, int origin_rank, char origin_file, int destiny_rank, char destiny_file, unsigned char capture, char special){
	CHESS_MOVE *new_move = malloc(sizeof(CHESS_MOVE));
	if(new_move != NULL){
		new_move->piece = toupper(piece);
		new_move->origin_rank = origin_rank;
		new_move->origin_file = origin_file;
		new_move->destiny_rank = destiny_rank;
		new_move->destiny_file = destiny_file;
		new_move->capture = capture;
		new_move->special = special;
		new_move->repeat = 0;
	}

	return new_move;
}

int compare_moves(CHESS_MOVE *move1, CHESS_MOVE *move2){
	if(move1 != NULL && move2 != NULL){
		if(move1->destiny_rank == move2->destiny_rank && move1->destiny_file == move2->destiny_file){
			if(move1->origin_rank == move2->origin_rank){
				if(move1->origin_file == move2->origin_file) return 3;
				return 2;
			}
			return 1;
		}
		return 0;
	}
	return -1;
}

int swap_moves(CHESS_MOVE **move1, CHESS_MOVE **move2){
	if(move1 != NULL && (*move1) != NULL && move2 != NULL && (*move2) != NULL){
		CHESS_MOVE *aux;
		aux = (*move1);
		(*move1) = (*move2);
		(*move2) = aux;
		return 1;
	}
	return 0;
}

int delete_move(CHESS_MOVE **move){
	if(move != NULL){
		if((*move) != NULL){
			free(*move);
			(*move) = NULL;
			return 0;
		}
		return 1;
	}
	return 2;
}

void print_move(CHESS_MOVE *move){
	if(move != NULL){
		int i = 0;
		char *output = (char*)malloc(sizeof(char) * 8);
		if(move->piece != 'P') output[i++] = move->piece;
		if(move->repeat == 1 || move->repeat == 3 || move->piece == 'P') output[i++] = move->origin_file;
		if(move->repeat == 2 || (move->repeat == 3 && move->special == '\0')) output[i++] = move->origin_rank + '0';
		if(move->capture) output[i++] = 'x';
		output[i++] = move->destiny_file;
		output[i++] = move->destiny_rank + '0';
		// Checa se o movimento a ser realizado é a promoção de um peão
		// E caso seja imprime para qual peça está sendo promovido
		if(move->special != 'E') output[i++] = move->special;
		output[i] = '\0';
		printf("%s", output);
		if(move->special == 'E') printf("e.p.");
		printf("\n");
		free(output);
	}

}
