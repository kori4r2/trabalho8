//  Ítalo Toble Silva - nUSP 8551910

#include <stdio.h>
#include <stdlib.h>
#include "my_strings.h"
#include "chess.h"
#include "hash_table.h"


/*------------------------------------------------------------------------------------------------------------
   current_state()
        Funcao que a partir da notacao fen de um tabuleiro obtem uma string com todas as informacoes menos turno,
	contagem de turno e contagem de meio-turnos
        - Parametros
          char* : string contendo a notacao fen
        - Retorno
          char* : string contendo apenas as informacoes necessarias
*/
char *current_state(char*);

int main(int argc, char *argv[]){

	unsigned char end = 0;
	int aux, print;
	char *input, *fen, turn;
	TABLE *table = create_table();
	HASH_TABLE *hash = create_hash_table(FIRST_SIZE);
	// Le a posicao inicial do tabuleiro
	read_table(stdin, table);

	// Simula um movimento anterior para avaliar se a posicao inicial ja contem uma condicao de fim de jogo
	input = dupe_move(table);
	// A variavel print armazena o resultado da jogada (uma jogada final retorna 0)
	if((print = move_piece(input, table)) > 0){
		// Caso seja uma jogada valida que permita a continuacao da partida
		if(print == 1){
			// Imprime a fen do tabuleiro apos a jogada
			fen = get_fen(table);
			printf("%s\n", fen);
			free(input);
			// Insere na tabela hash o estado atual
			insert_hash(&hash, current_state(fen));
		}
		// Enquanto a partida ainda nao tiver acabado
		while(!end){
			// A variavel turn armazena que jogador deve realizar uma jogada
			turn = cur_turn(table);
			// Se for a vez das brancas
			if(turn == WHITES_TURN){
				// Le a jogada a ser realizada da stdin
				if((input = my_get_line_valid(stdin, &aux)) != NULL){
					// Realiza a jogada se possivel, armazenando o resultado (movimento invalido retorna 2)
					print = move_piece(input, table);
					// Avalia se o fim de jogo foi atingido
					end = (print == 0);
				}else end = 1;
			}else if(turn == BLACKS_TURN){
				// Semelhante à jogada das brancas, mas obtem a jogada a partir da ia
				input = ai_move(table);
				print = move_piece(input, table);
				end = (print == 0);
			// Really, HOW?
			}else fprintf(stderr, "....what....how?\n");
			// Caso uma jogada valida nao tenha causado o fim de jogo
			if(!end && print == 1){
				// Imprime a fen da jogada resultando
				fen = get_fen(table);
				printf("%s\n", fen);
				// Insere o estado atual na tabela hash e avalia quantas repeticoes foram encontradas
				if(insert_hash(&hash, current_state(fen)) >= 3){
					// Caso sejam 3 ou mais, acaba o jogo com empate por tripla repeticao
					printf("Empate -- Tripla Repeticao\n");
					end = 1;
				}
			}
			// Libera a string lida
			free(input);
			input = NULL;
		}
		// Precaucao para evitar memory leaks
		if(input != NULL) free(input);
	// Se a situacao inicial ja for fim de jogo, apenas libera a string
	}else free(input);

	// Libera a memoria alocada
	delete_hash_table(&hash);
	delete_table(&table);

	return 0;
}

char *current_state(char *fen){
	if(fen == NULL) return NULL;

	// A variavel auxiliar copia a fen, pois strtok altera a string ao ser chamada
	char *aux = strdup(fen);
	char *tok = strtok(aux, " ");
	// String obtem o estado das pecas
	char *string = strdup(tok);
	int i;
	// Ignora o indicador de turno
	tok = strtok(NULL, " ");
	// Obtem as informacoes de roque e captura en passant
	for(i = 0 ; i < 2; i++){
		tok = strtok(NULL, " ");
		string = (char*)realloc(string, sizeof(char) * (strlen(string)+strlen(tok)+1));
		strcat(string, tok);
	}
	// Libera a memoria alocada e retorna a string avaliada
	free(aux);
	return string;
}
