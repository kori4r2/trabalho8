#include <stdio.h>
#include <stdlib.h>
#include "my_strings.h"
#include "chess.h"
#include "hash_table.h"

int main(int argc, char *argv[]){

	int aux;
	char *input;
	char *fen;
	TABLE *table = create_table();
	read_table(stdin, table);

	input = dupe_move(table);
	if(move_piece(input, table)){
		fen = get_fen(table);
		printf("%s\n", fen);
		free(input);
		while(((input = my_get_line_valid(stdin, &aux)) != NULL) && move_piece(input, table)){
			fen = get_fen(table);
			printf("%s\n", fen);
			free(input);
		}
		if(input != NULL) free(input);
	}else free(input);
	
	delete_table(&table);

	return 0;
}
