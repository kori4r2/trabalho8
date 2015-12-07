#include <stdio.h>
#include <stdlib.h>
#include "my_strings.h"
#include "chess.h"
#include "hash_table.h"

char *current_state(char*);

int main(int argc, char *argv[]){

	unsigned char end = 0;
	int aux;
	char *input;
	char *fen;
	TABLE *table = create_table();
	HASH_TABLE *hash = create_hash_table(FIRST_SIZE);
	read_table(stdin, table);

	input = dupe_move(table);
	if(move_piece(input, table)){
		fen = get_fen(table);
		printf("%s\n", fen);
		free(input);
		insert_hash(&hash, current_state(fen));
		while(!end && ((input = my_get_line_valid(stdin, &aux)) != NULL) && move_piece(input, table)){
			fen = get_fen(table);
			printf("%s\n", fen);
			free(input);
			input = NULL;
			if(insert_hash(&hash, current_state(fen)) >= 3){
				printf("Empate -- Tripla Repeticao\n");
				end = 1;
			}else{
				input = ai_move(table);
				fprintf(stderr, "ai move == %s\n", input);
				end = !move_piece(input, table);
				if(!end){
					fen = get_fen(table);
					printf("%s\n", fen);
					free(input);
					input = NULL;
				}
			}
		}
		if(input != NULL) free(input);
	}else free(input);

	delete_hash_table(&hash);
	delete_table(&table);

	return 0;
}

char *current_state(char *fen){
	if(fen == NULL) return NULL;

	char *aux = strdup(fen);
	char *tok = strtok(aux, " ");
	char *string = strdup(tok);
	int i;
	for(i = 0 ; i < 3; i++){
		tok = strtok(NULL, " ");
		string = (char*)realloc(string, sizeof(char) * (strlen(string)+strlen(tok)+1));
		strcat(string, tok);
	}
	free(aux);
	return string;
}
