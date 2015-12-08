#include "chess.h"

void move_rook(TABLE*, QUEUE*, PIECE*);
void move_knight(TABLE*, QUEUE*, PIECE*);
void move_bishop(TABLE*, QUEUE*, PIECE*);
void move_queen(TABLE*, QUEUE*, PIECE*);
void move_king(TABLE*, QUEUE*, PIECE*);
void move_pawn(TABLE*, QUEUE*, PIECE*);

typedef struct piece{
	char name;
	int priority;
	int rank;
	char file;
	int side;
	void (*move)(TABLE*, QUEUE*, PIECE*);
}PIECE;

typedef struct aux_node{
	PIECE *piece;
	struct aux_node *next;
}AUX_NODE;

struct list{
	struct aux_node *first;
	struct aux_node *last;
};

typedef struct table{
	PIECE ***grid;
	char turn;
	char *castling;
	char *en_passant;
	int half_turns;
	int cur_turn;
	PIECE *turn_king;
	char *fen;
}TABLE;

/*------------------------------------------------------------------------------------------------------------
   update_fen()
        Funcao que analisa o estado atual do tabuleiro e atualiza a notacao fen salva
        - Parametros
          TABLE* : ponteiro para o tabuleiro
        - Retorno
          int : 1 - parametro invalido passado; 0 - operacao bem-sucedida;
*/
int update_fen(TABLE*);

/*------------------------------------------------------------------------------------------------------------
   create_piece()
        Funcao que cria uma peca de acordo com as informacoes passadas, alocando a memoria necessaria
        - Parametros
          char : caractere indicativo da peca criada
          int : fila da peca criada
          char : coluna da peca criada
        - Retorno
          PIECE* : ponteiro para a peca criada; NULL caso erro de alocacao de memoria
*/
PIECE *create_piece(char, int, char);

/*------------------------------------------------------------------------------------------------------------
   delete_piece()
        Funcao que apaga uma peca criada, liberando a memoria alocada e setando NULL no seu ponteiro
        - Parametros
          PIECE** : endereco do ponteiro para a peca
        - Retorno
          void
*/
void delete_piece(PIECE**);

char cur_turn(TABLE *table){
	if(table != NULL) return table->turn;
	else return '\0';
}

// Essa funcao era originalmente a funcao de verificacao de cheque, por isso os comentarios, mas eles nao
// serao alterados pois ainda continuam validos
int is_attacked(TABLE *table, char file, int rank, int *a2){
	int i, j, enemy_side, check = 0, dupe = 0;
	PIECE *king = table->grid[8-rank][file-'a'];
	PIECE *aux;
	if(king == NULL){
		dupe = 1;
		king = create_piece(0, rank, file);
		king->side = table->turn_king->side;
	}
	enemy_side = (table->turn_king->side == WHITES_SIDE)? BLACKS_SIDE : WHITES_SIDE;

	*a2 = 0;

	// Checa se existe uma torre ou uma rainha atacando o rei para cima na coluna
	i = 1;
	while(king->rank+i < 8 && table->grid[8-(king->rank+i)][king->file-'a'] == NULL) i++;
	aux = ((king->rank+i <= 8) ? table->grid[8-(king->rank+i)][king->file-'a'] : NULL);
	// Checa se encontrou uma peça inimiga
	if(aux != NULL){
		// Checa se está imediatamente do lado do rei inimigo
		if(!check && i == 1 && aux->move == &move_king && aux->side == enemy_side) check = 1;
		if(!(*a2) && i == 1 && aux->move == &move_king && aux->side != enemy_side) *a2 = 1;
		// Caso não esteja, checa se há uma torre ou rainha ameaçando
		if(!check && (aux->move == &move_rook || aux->move == &move_queen) && aux->side == enemy_side) check = 1;
		if(!(*a2) && (aux->move == &move_rook || aux->move == &move_queen) && aux->side != enemy_side) *a2 = 1;
	}

	// Checa se existe uma torre ou uma rainha atacando o rei para baixo na coluna
	i = 1;
	while(king->rank-i > 1 && table->grid[8-(king->rank-i)][king->file-'a'] == NULL) i++;
	aux = ((king->rank-i >= 1) ? table->grid[8-(king->rank-i)][king->file-'a'] : NULL);
	// Checa se encontrou uma peça inimiga
	if(aux != NULL){
		// Checa se está imediatamente do lado do rei inimigo
		if(!check && i == 1 && aux->move == &move_king && aux->side == enemy_side) check = 1;
		if(!(*a2) && i == 1 && aux->move == &move_king && aux->side != enemy_side) *a2 = 1;
		// Caso não esteja, checa se há uma torre ou rainha ameaçando
		if(!check && (aux->move == &move_rook || aux->move == &move_queen) && aux->side == enemy_side) check = 1;
		if(!(*a2) && (aux->move == &move_rook || aux->move == &move_queen) && aux->side != enemy_side) *a2 = 1;
	}

	// Checa se existe uma torre ou uma rainha atacando o rei para a direta na linha
	i = 1;
	while(king->file+i < 'h' && table->grid[8-king->rank][(king->file+i)-'a'] == NULL) i++;
	aux = ((king->file+i <= 'h') ? table->grid[8-king->rank][(king->file+i)-'a'] : NULL);
	// Checa se encontrou uma peça inimiga
	if(aux != NULL){
		// Checa se está imediatamente do lado do rei inimigo
		if(!check && i == 1 && aux->move == &move_king && aux->side == enemy_side) check = 1;
		if(!(*a2) && i == 1 && aux->move == &move_king && aux->side != enemy_side) *a2 = 1;
		// Caso não esteja, checa se há uma torre ou rainha ameaçando
		if(!check && (aux->move == &move_rook || aux->move == &move_queen) && aux->side == enemy_side) check = 1;
		if(!(*a2) && (aux->move == &move_rook || aux->move == &move_queen) && aux->side != enemy_side) *a2 = 1;
	}

	// Checa se existe uma torre ou uma rainha atacando o rei para a esquerda na linha
	i = 1;
	while(king->file-i > 'a' && table->grid[8-king->rank][(king->file-i)-'a'] == NULL) i++;
	aux = ((king->file-i >= 'a') ? table->grid[8-king->rank][(king->file-i)-'a'] : NULL);
	// Checa se encontrou uma peça inimiga
	if(aux != NULL){
		// Checa se está imediatamente do lado do rei inimigo
		if(!check && i == 1 && aux->move == &move_king && aux->side == enemy_side) check = 1;
		if(!(*a2) && i == 1 && aux->move == &move_king && aux->side != enemy_side) *a2 = 1;
		// Caso não esteja, checa se há uma torre ou rainha ameaçando
		if(!check && (aux->move == &move_rook || aux->move == &move_queen) && aux->side == enemy_side) check = 1;
		if(!(*a2) && (aux->move == &move_rook || aux->move == &move_queen) && aux->side != enemy_side) *a2 = 1;
	}

	// Checa se existe um bispo ou uma rainha atacando o rei em uma diagonal
	i = 1;
	while(king->rank+i <= 8 && king->file+i <= 'h' && table->grid[8-(king->rank+i)][(king->file+i)-'a'] == NULL) i++;
	aux = ((king->rank+i <= 8 && king->file+i <= 'h') ? table->grid[8-(king->rank+i)][(king->file+i)-'a'] : NULL);
	if(!check && aux != NULL && (aux->move == &move_bishop || aux->move == &move_queen) && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && (aux->move == &move_bishop || aux->move == &move_queen) && aux->side != enemy_side) *a2 = 1;

	// Checa se existe um bispo ou uma rainha atacando o rei em uma diagonal
	i = 1;
	while(king->rank+i <= 8 && king->file-i >= 'a' && table->grid[8-(king->rank+i)][(king->file-i)-'a'] == NULL) i++;
	aux = ((king->rank+i <= 8 && king->file-i >= 'a') ? table->grid[8-(king->rank+i)][(king->file-i)-'a'] : NULL);
	if(!check && aux != NULL && (aux->move == &move_bishop || aux->move == &move_queen) && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && (aux->move == &move_bishop || aux->move == &move_queen) && aux->side != enemy_side) *a2 = 1;

	// Checa se existe um bispo ou uma rainha atacando o rei em uma diagonal
	i = 1;
	while(king->rank-i >= 1 && king->file+i <= 'h' && table->grid[8-(king->rank-i)][(king->file+i)-'a'] == NULL) i++;
	aux = ((king->rank-i >= 1 && king->file+i <= 'h') ? table->grid[8-(king->rank-i)][(king->file+i)-'a'] : NULL);
	if(!check && aux != NULL && (aux->move == &move_bishop || aux->move == &move_queen) && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && (aux->move == &move_bishop || aux->move == &move_queen) && aux->side != enemy_side) *a2 = 1;

	// Checa se existe um bispo ou uma rainha atacando o rei em uma diagonal
	i = 1;
	while(king->rank-i >= 1 && king->file-i >= 'a' && table->grid[8-(king->rank-i)][(king->file-i)-'a'] == NULL) i++;
	aux = ((king->rank-i >= 1 && king->file-i >= 'a') ? table->grid[8-(king->rank-i)][(king->file-i)-'a'] : NULL);
	if(!check && aux != NULL && (aux->move == &move_bishop || aux->move == &move_queen) && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && (aux->move == &move_bishop || aux->move == &move_queen) && aux->side != enemy_side) *a2 = 1;

	// Checa se a posição acima para a esquerda contém uma ameaça ao rei
	i = 1;
	j = -1;
	aux = ((king->rank+i <= 8 && king->file+j >= 'a') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(aux != NULL){
		if(aux->move == &move_king && aux->side == enemy_side) check = 1;
		if(aux->move == &move_king && aux->side != enemy_side) *a2 = 1;
		if(!check && table->turn_king->side == WHITES_SIDE && aux->move == &move_pawn && aux->side == enemy_side) check = 1;
		if(!(*a2) && table->turn_king->side == BLACKS_SIDE && aux->move == &move_pawn && aux->side != enemy_side) *a2 = 1;
	}

	// Checa se a posição acima para a direita contém uma ameaça ao rei
	i = 1;
	j = 1;
	aux = ((king->rank+i <= 8 && king->file+j <= 'h') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(aux != NULL){
		if(aux->move == &move_king && aux->side == enemy_side) check = 1;
		if(aux->move == &move_king && aux->side != enemy_side) *a2 = 1;
		if(!check && table->turn_king->side == WHITES_SIDE && aux->move == &move_pawn && aux->side == enemy_side) check = 1;
		if(!(*a2) && table->turn_king->side == BLACKS_SIDE && aux->move == &move_pawn && aux->side != enemy_side) *a2 = 1;
	}

	// Checa se a posição abaixo para a direita contém uma ameaça ao rei
	i = -1;
	j = 1;
	aux = ((king->rank+i > 0 && king->file+j <= 'h') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(aux != NULL){
		if(aux->move == &move_king && aux->side == enemy_side) check = 1;
		if(aux->move == &move_king && aux->side != enemy_side) *a2 = 1;
		if(!check && table->turn_king->side == BLACKS_SIDE && aux->move == &move_pawn && aux->side == enemy_side) check = 1;
		if(!(*a2) && table->turn_king->side == WHITES_SIDE && aux->move == &move_pawn && aux->side != enemy_side) *a2 = 1;
	}

	// Checa se a posição abaixo para a esquerda contém uma ameaça ao rei
	i = -1;
	j = -1;
	aux = ((king->rank+i > 0 && king->file+j >= 'a') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(aux != NULL){
		if(aux->move == &move_king && aux->side == enemy_side) check = 1;
		if(aux->move == &move_king && aux->side != enemy_side) *a2 = 1;
		if(!check && table->turn_king->side == BLACKS_SIDE && aux->move == &move_pawn && aux->side == enemy_side) check = 1;
		if(!(*a2) && table->turn_king->side == WHITES_SIDE && aux->move == &move_pawn && aux->side != enemy_side) *a2 = 1;
	}

	//Checa se há algum cavalo ameaçando o rei
	i = 2;
	j = -1;
	aux = ((king->rank+i <= 8 && king->file+j >= 'a') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(!check && aux != NULL && aux->move == &move_knight && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && aux->move == &move_knight && aux->side != enemy_side) *a2 = 1;

	j = 1;
	aux = ((king->rank+i <= 8 && king->file+j <= 'h') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(!check && aux != NULL && aux->move == &move_knight && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && aux->move == &move_knight && aux->side != enemy_side) *a2 = 1;

	i = 1;
	j = -2;
	aux = ((king->rank+i <= 8 && king->file+j >= 'a') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(!check && aux != NULL && aux->move == &move_knight && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && aux->move == &move_knight && aux->side != enemy_side) *a2 = 1;

	j = 2;
	aux = ((king->rank+i <= 8 && king->file+j <= 'h') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(!check && aux != NULL && aux->move == &move_knight && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && aux->move == &move_knight && aux->side != enemy_side) *a2 = 1;

	i = -1;
	j = -2;
	aux = ((king->rank+i > 0 && king->file+j >= 'a') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(!check && aux != NULL && aux->move == &move_knight && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && aux->move == &move_knight && aux->side != enemy_side) *a2 = 1;

	j = 2;
	aux = ((king->rank+i > 0 && king->file+j <= 'h') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(!check && aux != NULL && aux->move == &move_knight && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && aux->move == &move_knight && aux->side != enemy_side) *a2 = 1;

	i = -2;
	j = -1;
	aux = ((king->rank+i > 0 && king->file+j >= 'a') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(!check && aux != NULL && aux->move == &move_knight && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && aux->move == &move_knight && aux->side != enemy_side) *a2 = 1;

	j = 1;
	aux = ((king->rank+i > 0 && king->file+j <= 'h') ? table->grid[8-(king->rank+i)][(king->file+j)-'a'] : NULL);
	if(!check && aux != NULL && aux->move == &move_knight && aux->side == enemy_side) check = 1;
	if(!(*a2) && aux != NULL && aux->move == &move_knight && aux->side != enemy_side) *a2 = 1;

	if(dupe) delete_piece(&king);
	return check;
}

// Funcao que enfileira todos os movimentos possiveis da torre
void move_rook(TABLE *table, QUEUE *queue, PIECE *rook){
	int i, enemy_side, a2;
	PIECE *aux;
	enemy_side = (rook->side == WHITES_SIDE)? BLACKS_SIDE : WHITES_SIDE;

	// Move para a posição disponível mais distante para a esquerda na coluna
	i = 0;
	do{i++;}while(rook->file-i > 'a' && table->grid[8-rook->rank][(rook->file-i)-'a'] == NULL);
	// Checa se é uma posição válida, atribuindo o valor da posição a aux se necessário
	aux = ((rook->file-i >= 'a') ? table->grid[8-rook->rank][(rook->file-i)-'a'] : NULL);
	// Checa se encontrou uma peça inimiga
	if(aux != NULL && aux->side == enemy_side){
		// Verifica se capturar a peça deixa o rei em cheque
		table->grid[8-rook->rank][(rook->file-i)-'a'] = rook;
		table->grid[8-rook->rank][rook->file-'a'] = NULL;
		// Caso não coloque em cheque, enfilera o movimento
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(rook->name, rook->rank, rook->file, rook->rank, rook->file-i, 1, 0));
		// Desfaz a jogada
		table->grid[8-rook->rank][rook->file-'a'] = rook;
		table->grid[8-rook->rank][(rook->file-i)-'a'] = aux;
	}
	// Se tiver encontrado uma casa não vazia, decrementa i para indicar que já foi checada
	if(aux != NULL) i--;
	// Se possível
	if(rook->file-i >= 'a'){
		while(i > 0){
			// Volta todas as casas até a casa imediatamente do lado
			// E enfileira as jogadas possíveis
			table->grid[8-rook->rank][(rook->file-i)-'a'] = rook;
			table->grid[8-rook->rank][rook->file-'a'] = NULL;
			if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
				enqueue(queue, create_move(rook->name, rook->rank, rook->file, rook->rank, rook->file-i, 0, 0));
			table->grid[8-rook->rank][rook->file-'a'] = rook;
			table->grid[8-rook->rank][(rook->file-i)-'a'] = NULL;
			i--;
		}
	}

	
	// Faz o mesmo que foi descrito acima para as casa abaixo da torre
	i = 0;
	do{i++;}while(rook->rank-i > 1 && table->grid[8-(rook->rank-i)][rook->file-'a'] == NULL);
	aux = ((rook->rank-i >= 1) ? table->grid[8-(rook->rank-i)][rook->file-'a'] : NULL);
	if(aux != NULL && aux->side == enemy_side){
		table->grid[8-(rook->rank-i)][rook->file-'a'] = rook;
		table->grid[8-rook->rank][rook->file-'a'] = NULL;
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(rook->name, rook->rank, rook->file, rook->rank-i, rook->file, 1, 0));
		table->grid[8-rook->rank][rook->file-'a'] = rook;
		table->grid[8-(rook->rank-i)][rook->file-'a'] = aux;
	}
	// Se tiver encontrado uma casa não vazia, decrementa i para indicar que já foi checada
	if(aux != NULL) i--;
	if(rook->rank-i >= 1){
		while(i > 0){
			table->grid[8-(rook->rank-i)][rook->file-'a'] = rook;
			table->grid[8-rook->rank][rook->file-'a'] = NULL;
			if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
				enqueue(queue, create_move(rook->name, rook->rank, rook->file, rook->rank-i, rook->file, 0, 0));
			table->grid[8-rook->rank][rook->file-'a'] = rook;
			table->grid[8-(rook->rank-i)][rook->file-'a'] = NULL;
			i--;
		}
	}

	//Faz o mesmo para as casas acima da torre
	i = 1;
	// Nesse caso, porém, enfileira os movimentos à medida que sobe
	while(rook->rank+i <= 8 && table->grid[8-(rook->rank+i)][rook->file-'a'] == NULL){
		table->grid[8-(rook->rank+i)][rook->file-'a'] = rook;
		table->grid[8-rook->rank][rook->file-'a'] = NULL;
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(rook->name, rook->rank, rook->file, rook->rank+i, rook->file, 0, 0));
		table->grid[8-rook->rank][rook->file-'a'] = rook;
		table->grid[8-(rook->rank+i)][rook->file-'a'] = NULL;
		i++;
	}
	aux = ((rook->rank+i <= 8) ? table->grid[8-(rook->rank+i)][rook->file-'a'] : NULL);
	if(aux != NULL && aux->side == enemy_side){
		table->grid[8-(rook->rank+i)][rook->file-'a'] = rook;
		table->grid[8-rook->rank][rook->file-'a'] = NULL;
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(rook->name, rook->rank, rook->file, rook->rank+i, rook->file, 1, 0));
		table->grid[8-rook->rank][rook->file-'a'] = rook;
		table->grid[8-(rook->rank+i)][rook->file-'a'] = aux;
	}

	//Faz o mesmo para as casas à direita da torre
	i = 1;
	// Também enfileirando os movimentos à medida que se distancia da torre
	while(rook->file+i <= 'h' && table->grid[8-rook->rank][(rook->file+i)-'a'] == NULL){
		table->grid[8-rook->rank][(rook->file+i)-'a'] = rook;
		table->grid[8-rook->rank][rook->file-'a'] = NULL;
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(rook->name, rook->rank, rook->file, rook->rank, rook->file+i, 0, 0));
		table->grid[8-rook->rank][rook->file-'a'] = rook;
		table->grid[8-rook->rank][(rook->file+i)-'a'] = NULL;
		i++;
	}
	aux = ((rook->file+i <= 'h') ? table->grid[8-rook->rank][(rook->file+i)-'a'] : NULL);
	if(aux != NULL && aux->side == enemy_side){
		table->grid[8-rook->rank][(rook->file+i)-'a'] = rook;
		table->grid[8-rook->rank][rook->file-'a'] = NULL;
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(rook->name, rook->rank, rook->file, rook->rank, rook->file+i, 1, 0));
		table->grid[8-rook->rank][rook->file-'a'] = rook;
		table->grid[8-rook->rank][(rook->file+i)-'a'] = aux;
	}
}

// Funcao que enfileira todas as jogadas possiveis de um cavalo
void move_knight(TABLE *table, QUEUE *queue, PIECE *knight){
	int i, j, enemy_side, flag, capture, a2;
	PIECE *aux;
	enemy_side = (knight->side == WHITES_SIDE)? BLACKS_SIDE : WHITES_SIDE;

	j = -2;
	i = -1;
	// Checa o conteúdo da posição a ser avaliada
	aux = ((knight->rank+i >= 1 && knight->file+j >= 'a') ? table->grid[8-(knight->rank+i)][(knight->file+j)-'a'] : NULL);
	// Checa se é necessário capturar uma peça
	capture = (aux != NULL && aux->side == enemy_side);
	// flag será 0 quando a posição avaliada for uma peça aliada
	flag = (aux != NULL && !capture)? 0 : 1;
	// Caso necessário
	if(flag && (knight->rank+i >= 1 && knight->file+j >= 'a')){
		// Verifica se realizar o movimento deixa o rei em cheque
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = knight;
		table->grid[8-knight->rank][knight->file-'a'] = NULL;
		// Caso não coloque em cheque, enfilera o movimento
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(knight->name, knight->rank, knight->file, knight->rank+i, knight->file+j, capture, 0));
		// Desfaz a jogada
		table->grid[8-knight->rank][knight->file-'a'] = knight;
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = aux;
	}

	i = 1;
	aux = ((knight->rank+i <= 8 && knight->file+j >= 'a') ? table->grid[8-(knight->rank+i)][(knight->file+j)-'a'] : NULL);
	capture = (aux != NULL && aux->side == enemy_side);
	flag = (aux != NULL && !capture)? 0 : 1;
	if(flag && (knight->rank+i <= 8 && knight->file+j >= 'a')){
		// Verifica se realizar o movimento deixa o rei em cheque
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = knight;
		table->grid[8-knight->rank][knight->file-'a'] = NULL;
		// Caso não coloque em cheque, enfilera o movimento
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(knight->name, knight->rank, knight->file, knight->rank+i, knight->file+j, capture, 0));
		// Desfaz a jogada
		table->grid[8-knight->rank][knight->file-'a'] = knight;
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = aux;
	}

	j = -1;
	i = -2;
	aux = ((knight->rank+i >= 1 && knight->file+j >= 'a') ? table->grid[8-(knight->rank+i)][(knight->file+j)-'a'] : NULL);
	capture = (aux != NULL && aux->side == enemy_side);
	flag = (aux != NULL && !capture)? 0 : 1;
	if(flag && (knight->rank+i >= 1 && knight->file+j >= 'a')){
		// Verifica se realizar o movimento deixa o rei em cheque
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = knight;
		table->grid[8-knight->rank][knight->file-'a'] = NULL;
		// Caso não coloque em cheque, enfilera o movimento
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(knight->name, knight->rank, knight->file, knight->rank+i, knight->file+j, capture, 0));
		// Desfaz a jogada
		table->grid[8-knight->rank][knight->file-'a'] = knight;
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = aux;
	}

	i = 2;
	aux = ((knight->rank+i <= 8 && knight->file+j >= 'a') ? table->grid[8-(knight->rank+i)][(knight->file+j)-'a'] : NULL);
	capture = (aux != NULL && aux->side == enemy_side);
	flag = (aux != NULL && !capture)? 0 : 1;
	if(flag && (knight->rank+i <= 8 && knight->file+j >= 'a')){
		// Verifica se realizar o movimento deixa o rei em cheque
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = knight;
		table->grid[8-knight->rank][knight->file-'a'] = NULL;
		// Caso não coloque em cheque, enfilera o movimento
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(knight->name, knight->rank, knight->file, knight->rank+i, knight->file+j, capture, 0));
		// Desfaz a jogada
		table->grid[8-knight->rank][knight->file-'a'] = knight;
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = aux;
	}

	j = 1;
	i = -2;
	aux = ((knight->rank+i >= 1 && knight->file+j <= 'h') ? table->grid[8-(knight->rank+i)][(knight->file+j)-'a'] : NULL);
	capture = (aux != NULL && aux->side == enemy_side);
	flag = (aux != NULL && !capture)? 0 : 1;
	if(flag && (knight->rank+i >= 1 && knight->file+j <= 'h')){
		// Verifica se realizar o movimento deixa o rei em cheque
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = knight;
		table->grid[8-knight->rank][knight->file-'a'] = NULL;
		// Caso não coloque em cheque, enfilera o movimento
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(knight->name, knight->rank, knight->file, knight->rank+i, knight->file+j, capture, 0));
		// Desfaz a jogada
		table->grid[8-knight->rank][knight->file-'a'] = knight;
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = aux;
	}

	i = 2;
	aux = ((knight->rank+i <= 8 && knight->file+j <= 'h') ? table->grid[8-(knight->rank+i)][(knight->file+j)-'a'] : NULL);
	capture = (aux != NULL && aux->side == enemy_side);
	flag = (aux != NULL && !capture)? 0 : 1;
	if(flag && (knight->rank+i <= 8 && knight->file+j <= 'h')){
		// Verifica se realizar o movimento deixa o rei em cheque
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = knight;
		table->grid[8-knight->rank][knight->file-'a'] = NULL;
		// Caso não coloque em cheque, enfilera o movimento
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(knight->name, knight->rank, knight->file, knight->rank+i, knight->file+j, capture, 0));
		// Desfaz a jogada
		table->grid[8-knight->rank][knight->file-'a'] = knight;
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = aux;
	}

	j = 2;
	i = -1;
	aux = ((knight->rank+i >= 1 && knight->file+j <= 'h') ? table->grid[8-(knight->rank+i)][(knight->file+j)-'a'] : NULL);
	capture = (aux != NULL && aux->side == enemy_side);
	flag = (aux != NULL && !capture)? 0 : 1;
	if(flag && (knight->rank+i >= 1 && knight->file+j <= 'h')){
		// Verifica se realizar o movimento deixa o rei em cheque
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = knight;
		table->grid[8-knight->rank][knight->file-'a'] = NULL;
		// Caso não coloque em cheque, enfilera o movimento
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(knight->name, knight->rank, knight->file, knight->rank+i, knight->file+j, capture, 0));
		// Desfaz a jogada
		table->grid[8-knight->rank][knight->file-'a'] = knight;
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = aux;
	}

	i = 1;
	aux = ((knight->rank+i <= 8 && knight->file+j <= 'h') ? table->grid[8-(knight->rank+i)][(knight->file+j)-'a'] : NULL);
	capture = (aux != NULL && aux->side == enemy_side);
	flag = (aux != NULL && !capture)? 0 : 1;
	if(flag && (knight->rank+i <= 8 && knight->file+j <= 'h')){
		// Verifica se realizar o movimento deixa o rei em cheque
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = knight;
		table->grid[8-knight->rank][knight->file-'a'] = NULL;
		// Caso não coloque em cheque, enfilera o movimento
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(knight->name, knight->rank, knight->file, knight->rank+i, knight->file+j, capture, 0));
		// Desfaz a jogada
		table->grid[8-knight->rank][knight->file-'a'] = knight;
		table->grid[8-(knight->rank+i)][knight->file+j-'a'] = aux;
	}

}

// Funcao que efileira todas as jogadas possiveis de um bispo
void move_bishop(TABLE *table, QUEUE *queue, PIECE *bishop){
	int i, up, down, enemy_side, repeat, flag1, flag2, capture, a2;
	PIECE *aux1, *aux2;
	enemy_side = (bishop->side == WHITES_SIDE)? BLACKS_SIDE : WHITES_SIDE;

	// Move para as posições disponíveis mais distante para a esquerda e para cima ou para baixo
	i = 1;
	up = 1;
	down = 1;
	repeat = 1;
	// Enquanto estiver dentro dos limites do tabuleiro e alguma das diagonais tiver avançado
	while(bishop->file-i > 'a' && repeat){
		repeat = 0;
		// Incrementa a variável necessária para o movimento para cima
		if(bishop->rank + up < 8 && table->grid[8-(bishop->rank+up)][bishop->file-up-'a'] == NULL){
			up++;
			repeat = 1;
		}
		// Ou para baixo
		if(bishop->rank - down > 1 && table->grid[8-(bishop->rank-down)][bishop->file-down-'a'] == NULL){
			down++;
			repeat = 1;
		}
		i++;
	}
	// Checa se é uma posição válida, atribuindo o valor da posição a aux se necessário
	aux1 = ((bishop->file - up >= 'a' && bishop->rank + up <= 8) ? table->grid[8-(bishop->rank+up)][bishop->file-up-'a'] : NULL);
	aux2 = ((bishop->file - down >= 'a' && bishop->rank - down >= 1) ? table->grid[8-(bishop->rank-down)][bishop->file-down-'a'] : NULL);

	flag1 = (aux1 != NULL && aux1->side == enemy_side)? 1 : 0;
	flag2 = (aux2 != NULL && aux2->side == enemy_side)? 1 : 0;
	if(aux1 != NULL && !flag1) up--;
	if(aux2 != NULL && !flag2) down--;

	// Se possível
	while(bishop->file - i >= 'a' && bishop->file - i <= 'h' && i > 0){

		// Se down for menor ou igual a i, deve ser impresso o movimento da diagonal inferior
		if(down >= i && bishop->file-down >= 'a' && bishop->rank-down >= 1){
			// flag1 indica se é necessário checar captura de peças inimigas
			if(flag2){
				// Verifica se capturar a peça deixa o rei em cheque
				table->grid[8-(bishop->rank-down)][bishop->file-down-'a'] = bishop;
				table->grid[8-bishop->rank][bishop->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(bishop->name, bishop->rank, bishop->file, bishop->rank-down, bishop->file-down, 1, 0));
				// Desfaz a jogada
				table->grid[8-bishop->rank][bishop->file-'a'] = bishop;
				table->grid[8-(bishop->rank-down)][bishop->file-down-'a'] = aux2;
				flag2 = 0;
			}else{
				table->grid[8-(bishop->rank-down)][bishop->file-down-'a'] = bishop;
				table->grid[8-bishop->rank][bishop->file-'a'] = NULL;
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(bishop->name, bishop->rank, bishop->file, bishop->rank-down, bishop->file-down, 0, 0));
				table->grid[8-bishop->rank][bishop->file-'a'] = bishop;
				table->grid[8-(bishop->rank-down)][bishop->file-down-'a'] = NULL;
			}
			down--;
		}else if(down >= i) down--;;
		if(up > down && bishop->file-up >= 'a' && bishop->rank+up <= 8){
			// flag1 indica se é necessário checar captura de peças inimigas
			if(flag1){
				// Verifica se capturar a peça deixa o rei em cheque
				table->grid[8-(bishop->rank+up)][bishop->file-up-'a'] = bishop;
				table->grid[8-bishop->rank][bishop->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(bishop->name, bishop->rank, bishop->file, bishop->rank+up, bishop->file-up, 1, 0));
				// Desfaz a jogada
				table->grid[8-bishop->rank][bishop->file-'a'] = bishop;
				table->grid[8-(bishop->rank+up)][bishop->file-up-'a'] = aux1;
				flag1 = 0;
			}else{
				table->grid[8-(bishop->rank+up)][bishop->file-up-'a'] = bishop;
				table->grid[8-bishop->rank][bishop->file-'a'] = NULL;
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(bishop->name, bishop->rank, bishop->file, bishop->rank+up, bishop->file-up, 0, 0));
				table->grid[8-bishop->rank][bishop->file-'a'] = bishop;
				table->grid[8-(bishop->rank+up)][bishop->file-up-'a'] = NULL;
			}
			up--;
		}else if(up > down) up--;
		i--;
	}


	// A seguir checa todas as posições para a direita
	i = 1;
	up = 1;
	down = 1;
	repeat = 1;
	aux1 = NULL;
	aux2 = NULL;
	flag1 = flag2 = 1;
	// Enquanto estiver dentro dos limites do tabuleiro e alguma das diagonais tiver avançado
	while(bishop->file+i <= 'h' && repeat){
		repeat = 0;

		// Se aux2 ainda não tiver encontrado um valor anteriormente, atualiza com a posição atual
		if(!aux2)
			aux2 = ((bishop->file+down <= 'h' && bishop->rank-down >= 1) ? table->grid[8-(bishop->rank-down)][bishop->file+down-'a'] : NULL);
		// Caso seja necessário e seja uma posição válida
		if(flag2 && bishop->rank - down >= 1){
			// Verifica se o movimento é uma captura de peça
			capture = (aux2 != NULL && aux2->side == enemy_side)? 1 : 0;
			// Caso haja uma peça aliada na posição avaliada, muda a flag e não faz mais nada
			if(aux2 != NULL && !capture) flag2 = 0;
			else{
				// Verifica se realizar o movimento deixa o rei em cheque
				table->grid[8-(bishop->rank-down)][bishop->file+down-'a'] = bishop;
				table->grid[8-bishop->rank][bishop->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(bishop->name, bishop->rank, bishop->file, bishop->rank-down, bishop->file+down, capture, 0));
				// Desfaz a jogada
				table->grid[8-bishop->rank][bishop->file-'a'] = bishop;
				table->grid[8-(bishop->rank-down)][bishop->file+down-'a'] = aux2;
				down++;
				if(aux2 != NULL) flag2 = 0;
				repeat = 1;
			}
		}
		// Se aux1 ainda não tiver encontrado um valor anteriormente, atualiza com a posição atual
		if(!aux1)
			aux1 = ((bishop->file + up <= 'h' && bishop->rank + up <= 8) ? table->grid[8-(bishop->rank+up)][bishop->file+up-'a'] : NULL);
		// Caso seja necessário e seja uma posição válida
		if(flag1 && bishop->rank + up <= 8){
			// Verifica se o movimento é uma captura de peça
			capture = (aux1 != NULL && aux1->side == enemy_side)? 1 : 0;
			// Caso haja uma peça aliada na posição avaliada, muda a flag e não faz mais nada
			if(aux1 != NULL && !capture) flag1 = 0;
			else{
				// Verifica se realizar o movimento deixa o rei em cheque
				table->grid[8-(bishop->rank+up)][bishop->file+up-'a'] = bishop;
				table->grid[8-bishop->rank][bishop->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(bishop->name, bishop->rank, bishop->file, bishop->rank+up, bishop->file+up, capture, 0));
				// Desfaz a jogada
				table->grid[8-bishop->rank][bishop->file-'a'] = bishop;
				table->grid[8-(bishop->rank+up)][bishop->file+up-'a'] = aux1;
				up++;
				if(aux1 != NULL) flag1 = 0;
				repeat = 1;
			}
		}

		i++;
	}
}

// Funcao que enfileira todas as jogadas possiveis da rainha
void move_queen(TABLE *table, QUEUE *queue, PIECE *queen){
	int i, up, down, side, enemy_side, repeat, flag1, flag2, flag3, capture, a2;
	PIECE *aux1, *aux2, *aux3;
	enemy_side = (queen->side == WHITES_SIDE)? BLACKS_SIDE : WHITES_SIDE;

	// Move para as posições disponíveis mais distante para a esquerda e para cima ou para baixo
	i = 1;
	up = 1;
	down = 1;
	side = 1;
	repeat = 1;
	// Enquanto estiver dentro dos limites do tabuleiro e alguma das 3 direções estiver avançando 
	while(queen->file-i > 'a' && repeat){
		repeat = 0;
		// Incrementa a variável necessária para o movimento para cima
		if(queen->rank + up < 8 && table->grid[8-(queen->rank+up)][queen->file-up-'a'] == NULL){
			up++;
			repeat = 1;
		}
		// Para o lado
		if(table->grid[8-queen->rank][queen->file-side-'a'] == NULL){
			side++;
			repeat = 1;
		}
		// Ou para baixo
		if(queen->rank - down > 1 && table->grid[8-(queen->rank - down)][queen->file-down-'a'] == NULL){
			down++;
			repeat = 1;
		}
		i++;
	}
	// Checa se é uma posição válida, atribuindo o valor da posição a aux se necessário
	aux1 = ((queen->file - up >= 'a' && queen->rank + up <= 8) ? table->grid[8-(queen->rank+up)][queen->file-up-'a'] : NULL);
	aux2 = ((queen->file - down >= 'a' && queen->rank - down >= 1) ? table->grid[8-(queen->rank-down)][queen->file-down-'a'] : NULL);
	aux3 = ((queen->file - side >= 'a') ? table->grid[8-queen->rank][queen->file-side-'a'] : NULL);

	flag1 = (aux1 != NULL && aux1->side == enemy_side)? 1 : 0;
	flag2 = (aux2 != NULL && aux2->side == enemy_side)? 1 : 0;
	flag3 = (aux3 != NULL && aux3->side == enemy_side)? 1 : 0;
	if(aux1 != NULL && !flag1) up--;
	if(aux2 != NULL && !flag2) down--;
	if(aux3 != NULL && !flag3) side--;

	// Se possível
	while(queen->file - i >= 'a' && queen->file - i <= 'h' && i > 0){

		// Se down for menor ou igual a i, deve ser impresso o movimento da diagonal inferior
		if(down >= i && queen->file-down >= 'a' && queen->rank-down >= 1){
			// flag2 indica se é necessário checar captura de peças inimigas
			if(flag2){
				// Verifica se capturar a peça deixa o rei em cheque
				table->grid[8-(queen->rank-down)][queen->file-down-'a'] = queen;
				table->grid[8-queen->rank][queen->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank-down, queen->file-down, 1, 0));
				// Desfaz a jogada
				table->grid[8-queen->rank][queen->file-'a'] = queen;
				table->grid[8-(queen->rank-down)][queen->file-down-'a'] = aux2;
				flag2 = 0;
			}else{
				table->grid[8-(queen->rank-down)][queen->file-down-'a'] = queen;
				table->grid[8-queen->rank][queen->file-'a'] = NULL;
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank-down, queen->file-down, 0, 0));
				table->grid[8-queen->rank][queen->file-'a'] = queen;
				table->grid[8-(queen->rank-down)][queen->file-down-'a'] = NULL;
			}
			down--;
		}else if(down >= i) down--;

		// Se side for menor que down, deve ser impresso o movimento da horizontal
		if(side > down && queen->file-side >= 'a'){
			// flag3 indica se é necessário checar captura de peças inimigas
			if(flag3){
				// Verifica se capturar a peça deixa o rei em cheque
				table->grid[8-queen->rank][queen->file-side-'a'] = queen;
				table->grid[8-queen->rank][queen->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank, queen->file-side, 1, 0));
				// Desfaz a jogada
				table->grid[8-queen->rank][queen->file-'a'] = queen;
				table->grid[8-queen->rank][queen->file-side-'a'] = aux3;
				flag3 = 0;
			}else{
				table->grid[8-queen->rank][queen->file-side-'a'] = queen;
				table->grid[8-queen->rank][queen->file-'a'] = NULL;
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank, queen->file-side, 0, 0));
				table->grid[8-queen->rank][queen->file-'a'] = queen;
				table->grid[8-queen->rank][queen->file-side-'a'] = NULL;
			}
			side--;
		}else if(side > down) side--;

		// Se up for menor que side, deve ser impresso o movimento da horizontal
		if(up > side && queen->file-up >= 'a' && queen->rank+up <= 8){
			// flag1 indica se é necessário checar captura de peças inimigas
			if(flag1){
				// Verifica se capturar a peça deixa o rei em cheque
				table->grid[8-(queen->rank+up)][queen->file-up-'a'] = queen;
				table->grid[8-queen->rank][queen->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank+up, queen->file-up, 1, 0));
				// Desfaz a jogada
				table->grid[8-queen->rank][queen->file-'a'] = queen;
				table->grid[8-(queen->rank+up)][queen->file-up-'a'] = aux1;
				flag1 = 0;
			}else{
				table->grid[8-(queen->rank+up)][queen->file-up-'a'] = queen;
				table->grid[8-queen->rank][queen->file-'a'] = NULL;
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank+up, queen->file-up, 0, 0));
				table->grid[8-queen->rank][queen->file-'a'] = queen;
				table->grid[8-(queen->rank+up)][queen->file-up-'a'] = NULL;
			}
			up--;
		}else if(up > side) up--;
		i--;
	}

	// A seguir checa as posições para baixo(mesma lógica da torre)
	i = 0;
	do{i++;}while(queen->rank-i > 1 && table->grid[8-(queen->rank-i)][queen->file-'a'] == NULL);
	aux1 = ((queen->rank-i >= 1) ? table->grid[8-(queen->rank-i)][queen->file-'a'] : NULL);
	if(aux1 != NULL && aux1->side == enemy_side){
		table->grid[8-(queen->rank-i)][queen->file-'a'] = queen;
		table->grid[8-queen->rank][queen->file-'a'] = NULL;
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank-i, queen->file, 1, 0));
		table->grid[8-queen->rank][queen->file-'a'] = queen;
		table->grid[8-(queen->rank-i)][queen->file-'a'] = aux1;
	}
	if(aux1 != NULL) i--;
	if(queen->rank-i >= 1){
		while(i > 0){
			table->grid[8-(queen->rank-i)][queen->file-'a'] = queen;
			table->grid[8-queen->rank][queen->file-'a'] = NULL;
			if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
				enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank-i, queen->file, 0, 0));
			table->grid[8-queen->rank][queen->file-'a'] = queen;
			table->grid[8-(queen->rank-i)][queen->file-'a'] = NULL;
			i--;
		}
	}

	//Faz o mesmo para as casas acima da rainha
	i = 1;
	// Nesse caso, porém, enfileira os movimentos à medida que sobe
	while(queen->rank+i <= 8 && table->grid[8-(queen->rank+i)][queen->file-'a'] == NULL){
		table->grid[8-(queen->rank+i)][queen->file-'a'] = queen;
		table->grid[8-queen->rank][queen->file-'a'] = NULL;
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank+i, queen->file, 0, 0));
		table->grid[8-queen->rank][queen->file-'a'] = queen;
		table->grid[8-(queen->rank+i)][queen->file-'a'] = NULL;
		i++;
	}
	aux1 = ((queen->rank+i <= 8) ? table->grid[8-(queen->rank+i)][queen->file-'a'] : NULL);
	if(aux1 != NULL && aux1->side == enemy_side){
		table->grid[8-(queen->rank+i)][queen->file-'a'] = queen;
		table->grid[8-queen->rank][queen->file-'a'] = NULL;
		if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank+i, queen->file, 1, 0));
		table->grid[8-queen->rank][queen->file-'a'] = queen;
		table->grid[8-(queen->rank+i)][queen->file-'a'] = aux1;
	}


	// A seguir checa todas as posições para a direita
	i = 1;
	up = 1;
	down = 1;
	side = 1;
	repeat = 1;
	aux1 = NULL;
	aux2 = NULL;
	aux3 = NULL;
	flag1 = flag2 = flag3 = 1;
	// Enquanto estiver dentro dos limites do tabuleiro e alguma das diagonais tiver avançado
	while(queen->file+i <= 'h' && repeat){
		repeat = 0;

		// Se aux2 ainda não tiver encontrado um valor anteriormente, atualiza com a posição atual
		if(!aux2)
			aux2 = ((queen->file+down <= 'h' && queen->rank-down >= 1) ? table->grid[8-(queen->rank-down)][queen->file+down-'a'] : NULL);
		// Caso seja necessário e seja uma posição válida
		if(flag2 && queen->rank - down >= 1){
			// Verifica se o movimento é uma captura de peça
			capture = (aux2 != NULL && aux2->side == enemy_side)? 1 : 0;
			// Caso haja uma peça aliada na posição avaliada, muda a flag e não faz mais nada
			if(aux2 != NULL && !capture) flag2 = 0;
			else{
				// Verifica se realizar o movimento deixa o rei em cheque
				table->grid[8-(queen->rank-down)][queen->file+down-'a'] = queen;
				table->grid[8-queen->rank][queen->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank-down, queen->file+down, capture, 0));
				// Desfaz a jogada
				table->grid[8-queen->rank][queen->file-'a'] = queen;
				table->grid[8-(queen->rank-down)][queen->file+down-'a'] = aux2;
				down++;
				if(aux2 != NULL) flag2 = 0;
				repeat = 1;
			}
		}
		// Se aux3 ainda não tiver encontrado um valor anteriormente, atualiza com a posição atual
		if(!aux3)
			aux3 = ((queen->file+side <= 'h') ? table->grid[8-queen->rank][queen->file+side-'a'] : NULL);
		// Caso seja necessário e seja uma posição válida
		if(flag3){
			// Verifica se o movimento é uma captura de peça
			capture = (aux3 != NULL && aux3->side == enemy_side)? 1 : 0;
			// Caso haja uma peça aliada na posição avaliada, muda a flag e não faz mais nada
			if(aux3 != NULL && !capture) flag3 = 0;
			else{
				// Verifica se realizar o movimento deixa o rei em cheque
				table->grid[8-queen->rank][queen->file+side-'a'] = queen;
				table->grid[8-queen->rank][queen->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank, queen->file+side, capture, 0));
				// Desfaz a jogada
				table->grid[8-queen->rank][queen->file-'a'] = queen;
				table->grid[8-queen->rank][queen->file+side-'a'] = aux3;
				side++;
				if(aux3 != NULL) flag3 = 0;
				repeat = 1;
			}
		}
		// Se aux1 ainda não tiver encontrado um valor anteriormente, atualiza com a posição atual
		if(!aux1)
			aux1 = ((queen->file + up <= 'h' && queen->rank + up <= 8) ? table->grid[8-(queen->rank+up)][queen->file+up-'a'] : NULL);
		// Caso seja necessário e seja uma posição válida
		if(flag1 && queen->rank + up <= 8){
			// Verifica se o movimento é uma captura de peça
			capture = (aux1 != NULL && aux1->side == enemy_side)? 1 : 0;
			// Caso haja uma peça aliada na posição avaliada, muda a flag e não faz mais nada
			if(aux1 != NULL && !capture) flag1 = 0;
			else{
				// Verifica se realizar o movimento deixa o rei em cheque
				table->grid[8-(queen->rank+up)][queen->file+up-'a'] = queen;
				table->grid[8-queen->rank][queen->file-'a'] = NULL;
				// Caso não coloque em cheque, enfilera o movimento
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(queen->name, queen->rank, queen->file, queen->rank+up, queen->file+up, capture, 0));
				// Desfaz a jogada
				table->grid[8-queen->rank][queen->file-'a'] = queen;
				table->grid[8-(queen->rank+up)][queen->file+up-'a'] = aux1;
				up++;
				if(aux1 != NULL) flag1 = 0;
				repeat = 1;
			}
		}

		i++;
	}

}

// Funcao que enfileira todas as jogadas possiveis do rei
void move_king(TABLE *table, QUEUE *queue, PIECE *king){
	int i, j, enemy_side, capture, flag, castling_queen, castling_king, a2;
	char cast_queen_flag, cast_king_flag;
	PIECE *aux;

	// Check castlings if necessary
	cast_queen_flag = (king->side == WHITES_SIDE)? 'Q' : 'q';
	cast_king_flag = (king->side == WHITES_SIDE)? 'K' : 'k';
	castling_queen = 0;
	castling_king = 0;
	enemy_side = (king->side == WHITES_SIDE)? BLACKS_SIDE : WHITES_SIDE;
	if(table->castling[0] != '-'){
		// Percorre a string de roque
		for(i = 0; table->castling[i] != '\0'; i++){
			// Se for encontrado o indicador de roque da rainha
			if(table->castling[i] == cast_queen_flag){
				castling_queen = 1;
				table->grid[8-king->rank][king->file-'a'] = NULL;
				// Percorre as 3 casas à esquerda analisando se algo impossibilita o roque
				for(j = 0, flag = 1; j < 3 && flag; j++){
					king->file--;
					if(table->grid[8-king->rank][king->file-'a'] != NULL){
						castling_queen = 0;
						flag = 0;
					}else if(j < 2){
						table->grid[8-king->rank][king->file-'a'] = king;
						if(is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2)){
							castling_queen = 0;
							flag = 0;
						}
						table->grid[8-king->rank][king->file-'a'] = NULL;
					}
				}
				king->file += j;
				table->grid[8-king->rank][king->file-'a'] = king;
			// Se for encontrado o indicador de roque do rei
			}else if(table->castling[i] == cast_king_flag){
				castling_king = 1;
				table->grid[8-king->rank][king->file-'a'] = NULL;
				// Percorre as 2 casas à direita analisando se algo impossibilita o roque
				for(j = 0, flag = 1; j < 2 && flag; j++){
					king->file++;
					if(table->grid[8-king->rank][king->file-'a'] != NULL){
						castling_king = 0;
						flag = 0;
					}else{
						table->grid[8-king->rank][king->file-'a'] = king;
						if(is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2)){
							castling_king = 0;
							flag = 0;
						}
						table->grid[8-king->rank][king->file-'a'] = NULL;
					}
				}
				king->file -= j;
				table->grid[8-king->rank][king->file-'a'] = king;
			}
		}
	}

	// Castling left
	if(castling_queen) enqueue(queue, create_move(king->name, king->rank, king->file, king->rank, king->file-2, 0, 0));

	// Checa as três posições à esquerda do rei
	j = -1;
	if(king->file+j >= 'a'){
		table->grid[8-king->rank][king->file-'a'] = NULL;
		king->file += j;
		king->rank--;
		for(i = -1; i <= 1; i++){
			aux = (king->rank >= 1 && king->rank <= 8)? table->grid[8-king->rank][king->file-'a'] : NULL;
			capture = (aux != NULL && aux->side == enemy_side);
			flag = !(aux != NULL && !capture);
			if(flag && king->rank >= 1 && king->rank <= 8){
				table->grid[8-king->rank][king->file-'a'] = king;
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(king->name, king->rank-i, king->file-j, king->rank, king->file, capture, 0));
				table->grid[8-king->rank][king->file-'a'] = aux;
			}
			king->rank++;
		}
		king->rank -= 2;
		king->file -= j;
		table->grid[8-king->rank][king->file-'a'] = king;
	}

	// Checa as posições abaixo e acima do rei
	i = -1;
	if(king->rank+i >= 1){
		table->grid[8-king->rank][king->file-'a'] = NULL;
		king->rank--;
		aux = (king->rank >= 1)? table->grid[8-king->rank][king->file-'a'] : NULL;
		capture = (aux != NULL && aux->side == enemy_side);
		flag = !(aux != NULL && !capture);
		if(flag && king->rank >= 1){
			table->grid[8-king->rank][king->file-'a'] = king;
			if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
				enqueue(queue, create_move(king->name, king->rank-i, king->file, king->rank, king->file, capture, 0));
			table->grid[8-king->rank][king->file-'a'] = aux;
		}
		king->rank++;
		table->grid[8-king->rank][king->file-'a'] = king;
	}
	i = 1;
	if(king->rank+i <= 8){
		table->grid[8-king->rank][king->file-'a'] = NULL;
		king->rank++;
		aux = (king->rank <= 8)? table->grid[8-king->rank][king->file-'a'] : NULL;
		capture = (aux != NULL && aux->side == enemy_side);
		flag = !(aux != NULL && !capture);
		if(flag && king->rank <= 8){
			table->grid[8-king->rank][king->file-'a'] = king;
			if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
				enqueue(queue, create_move(king->name, king->rank-i, king->file, king->rank, king->file, capture, 0));
			table->grid[8-king->rank][king->file-'a'] = aux;
		}
		king->rank--;
		table->grid[8-king->rank][king->file-'a'] = king;
	}


	// Checa as três posições À direita do rei
	j = 1;
	if(king->file+j <= 'h'){
		table->grid[8-king->rank][king->file-'a'] = NULL;
		king->file += j;
		king->rank--;
		for(i = -1; i <= 1; i++){
			aux = (king->rank >= 1 && king->rank <= 8)? table->grid[8-king->rank][king->file-'a'] : NULL;
			capture = (aux != NULL && aux->side == enemy_side);
			flag = !(aux != NULL && !capture);
			if(flag && king->rank >= 1 && king->rank <= 8){
				table->grid[8-king->rank][king->file-'a'] = king;
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(king->name, king->rank-i, king->file-j, king->rank, king->file, capture, 0));
				table->grid[8-king->rank][king->file-'a'] = aux;
			}
			king->rank++;
		}
		king->rank -= 2;
		king->file -= j;
		table->grid[8-king->rank][king->file-'a'] = king;
	}

	//Castling right
	if(castling_king) enqueue(queue, create_move(king->name, king->rank, king->file, king->rank, king->file+2, 0, 0));
}

// Funcao que enfileira todas as jogadas possiveis do peao
void move_pawn(TABLE *table, QUEUE *queue, PIECE *pawn){
	int enemy_side, start, finish, i, j, mirror, capture, en_passant_rank, flag, a2;
	char en_passant_file, special = '\0';
	PIECE *aux;

	enemy_side = (pawn->side == WHITES_SIDE)? BLACKS_SIDE : WHITES_SIDE;
	start = (pawn->side == WHITES_SIDE)? 2 : 7;
	finish = (pawn->side == WHITES_SIDE)? 8 : 1;
	// A variável mirror serve para fazer a análise independentemente do lado ao qual o peão pertence
	mirror = (pawn->side == WHITES_SIDE)? 1 : -1;
	
	if(table->en_passant[0] != '-'){
		en_passant_file = table->en_passant[0];
		en_passant_rank = table->en_passant[1]-'0';
	}else{
		en_passant_rank = 0;
		en_passant_file = 'i';
	}

	// Verifica a captura para a casa à esquerda
	if(pawn->file-1 >= 'a' && pawn->rank+mirror <= 8 && pawn->rank+mirror >= 1){
		special = (pawn->file-1 == en_passant_file && pawn->rank+mirror == en_passant_rank)? 'E' : '\0';
		flag = (special != '\0')? 1 : 0;
		aux = table->grid[8-(pawn->rank+(mirror*(1-flag)))][pawn->file-1-'a'];
		capture = (aux != NULL && aux->side == enemy_side);
		if(flag && table->grid[8-(pawn->rank+mirror)][pawn->file-1-'a'] != NULL) capture = 0;
		if(capture){
			table->grid[8-(pawn->rank+mirror)][pawn->file-1-'a'] = pawn;
			table->grid[8-pawn->rank][pawn->file-'a'] = NULL;
			if(flag) table->grid[8-(pawn->rank)][pawn->file-1-'a'] = NULL;
			if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2)){
				// Se atingir o final enfileira todas as jogas possíveis
				if(pawn->rank+mirror == finish){
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file-1,
																capture, 'N'));
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file-1,
																capture, 'B'));
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file-1,
																capture, 'R'));
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file-1,
																capture, 'Q'));
				}else{
				// Caso contrário, só há uma jogada possível
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file-1,
																capture, special));
				}
			}
			table->grid[8-(pawn->rank)][pawn->file-'a'] = pawn;
			table->grid[8-(pawn->rank+mirror)][pawn->file-1-'a'] = NULL;
			table->grid[8-(pawn->rank+(mirror*(1-flag)))][pawn->file-1-'a'] = aux;
		}
	}

	// j indica se primeiro deve-se checar o movimento de duas casas ou não. Se j = 1, o movimento de uma casa é checado primeiro
	j = (mirror > 0)? 1 : 0;
	for(i = 0; i < 2; i++){
		if(!j && pawn->rank == start && table->grid[8-(pawn->rank+mirror)][pawn->file-'a'] == NULL){
			aux = table->grid[8-(pawn->rank+(mirror*2))][pawn->file-'a'];
			flag = (aux == NULL);
			if(flag){
				table->grid[8-(pawn->rank+(mirror*2))][pawn->file-'a'] = pawn;
				table->grid[8-(pawn->rank)][pawn->file-'a'] = NULL;
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+(mirror*2), pawn->file, 0, '\0'));
				table->grid[8-(pawn->rank)][pawn->file-'a'] = pawn;
				table->grid[8-(pawn->rank+(mirror*2))][pawn->file-'a'] = aux;
			}
		}else if(j && pawn->rank+mirror <= 8 && pawn->rank+mirror >= 1){
			aux = table->grid[8-(pawn->rank+mirror)][pawn->file-'a'];
			flag = (aux == NULL);
			if(flag){
				table->grid[8-(pawn->rank+mirror)][pawn->file-'a'] = pawn;
				table->grid[8-(pawn->rank)][pawn->file-'a'] = NULL;
				if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2)){
					// Se atingir o final enfileira todas as jogas possíveis
					if(pawn->rank+mirror == finish){
						enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file, 0, 'N'));
						enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file, 0, 'B'));
						enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file, 0, 'R'));
						enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file, 0, 'Q'));
					}else{
					// Caso contrário, só há uma jogada possível
						enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file, 0, '\0'));
					}
				}
				table->grid[8-(pawn->rank)][pawn->file-'a'] = pawn;
				table->grid[8-(pawn->rank+mirror)][pawn->file-'a'] = aux;
			}	
		}
		j = !j;
	}

	// Verifica a captura de casa à direita
	if(pawn->file+1 <= 'h' && pawn->rank+mirror <= 8 && pawn->rank+mirror >= 1){
		special = (pawn->file+1 == en_passant_file && pawn->rank+mirror == en_passant_rank)? 'E' : '\0';
		flag = (special != '\0')? 1 : 0;
		aux = table->grid[8-(pawn->rank+(mirror*(1-flag)))][pawn->file+1-'a'];
		capture = (aux != NULL && aux->side == enemy_side);
		if(flag && table->grid[8-(pawn->rank+mirror)][pawn->file+1-'a'] != NULL) capture = 0;
		if(capture){
			table->grid[8-(pawn->rank+mirror)][pawn->file-'a'+1] = pawn;
			table->grid[8-(pawn->rank)][pawn->file-'a'] = NULL;
			if(flag) table->grid[8-(pawn->rank)][pawn->file+1-'a'] = NULL;
			if(!is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2)){
				// Se atingir o final enfileira todas as jogas possíveis
				if(pawn->rank+mirror == finish){
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file+1,
																capture, 'N'));
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file+1,
																capture, 'B'));
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file+1,
																capture, 'R'));
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file+1,
																capture, 'Q'));
				}else{
				// Caso contrário, só há uma jogada possível
					enqueue(queue, create_move(pawn->name, pawn->rank, pawn->file, pawn->rank+mirror, pawn->file+1,
																capture, special));
				}
			}
			table->grid[8-(pawn->rank)][pawn->file-'a'] = pawn;
			table->grid[8-(pawn->rank+mirror)][pawn->file+1-'a'] = NULL;
			table->grid[8-(pawn->rank+(mirror*(1-flag)))][pawn->file+1-'a'] = aux;
		}
	}
}

PIECE *create_piece(char name, int rank, char file){
	PIECE *piece = (PIECE*)malloc(sizeof(PIECE));
	// Se a alocacao de memoria for bem sucedida
	if(piece != NULL){
		// Atribui nome e posicao da peca
		piece->name = name;
		piece->rank = rank;
		piece->file = file;
		// Avalia se a peca é preta ou branca
		piece->side = (isupper(name)) ? WHITES_SIDE : BLACKS_SIDE;
		// Analisa o nome da peca para atribuir sua funcao de movimento
		switch(name){
			case W_ROOK:
			case B_ROOK:
				piece->move = &move_rook;
				piece->priority = 3;
				break;
			case W_KNIGHT:
			case B_KNIGHT:
				piece->move = &move_knight;
				piece->priority = 1;
				break;
			case W_BISHOP:
			case B_BISHOP:
				piece->move = &move_bishop;
				piece->priority = 2;
				break;
			case W_QUEEN:
			case B_QUEEN:
				piece->move = &move_queen;
				piece->priority = 4;
				break;
			case W_KING:
			case B_KING:
				piece->move = &move_king;
				piece->priority = 5;
				break;
			case W_PAWN:
			case B_PAWN:
				piece->move = &move_pawn;
				piece->priority = 0;
				break;
		}
	}

	return piece;
}

// Funcao que compara a prioridade das pecas para organizacao da lista
int compare_priority(PIECE *piece1, PIECE *piece2){
	// Verifica se foram passadas peças válidas
	if(piece1 != NULL && piece2 != NULL){
		// Compara as prioridades das peças
		if(piece1->priority < piece2->priority) return -1;
		else if(piece1->priority > piece2->priority) return 1;
		else{
			// A peça na coluna(file) de menor valor tem maior prioridade
			if(piece1->file < piece2->file) return -1;
			else if(piece1->file > piece2->file) return 1;
			else{
				// A peça na linha(rank) de menor valor tem maior prioridade
				if(piece1->rank < piece2->rank) return -1;
				else return 1;
			}
		}
	}
	return 0;
}

// Funcao que cria um nó auxiliar (nó da lista de pecas)
AUX_NODE *create_aux_node(PIECE *piece){
	AUX_NODE *node = NULL;
	if(piece != NULL){
		node = (AUX_NODE*)malloc(sizeof(AUX_NODE));
		if(node != NULL){
			node->piece = piece;
			node->next = node;
		}
	}
	return node;
}

// Funcao que insere uma peca na lista de pecas de maneira ordenada
int insert_list(PIECE_LIST *list, PIECE *piece){
	if(list != NULL && piece != NULL){
		AUX_NODE *new_node;
		// Se nao houver nenhum item na lista, apenas cria o primeiro item
		if(list->first == NULL) list->last = list->first = create_aux_node(piece);
		// Caso o item inserido seja o novo primeiro item, faz as atribuicoes necessarias
		else if(compare_priority(piece, list->first->piece) < 0){
			new_node = create_aux_node(piece);
			new_node->next = list->first;
			list->last->next = new_node;
			list->first = new_node;
		// Caso contrario
		}else{
			AUX_NODE *prev_node = list->first;
			// Percorre a lista procurando a posicao de insercao do nó
			while(compare_priority(piece, prev_node->next->piece) > 0 && prev_node->next != list->first)
				prev_node = prev_node->next;
			// E o insere na lista
			new_node = create_aux_node(piece);
			new_node->next = prev_node->next;
			prev_node->next = new_node;
			if(new_node->next == list->first) list->last = new_node;
		}
		return 0;
	}
	return 1;
}

// Funcao que cria uma lista de pecas vazia
PIECE_LIST *create_empty_list(void){
	PIECE_LIST *list = (PIECE_LIST*)malloc(sizeof(PIECE_LIST));
	if(list != NULL){
		list->first = NULL;
		list->last = NULL;
	}
	return list;
}

PIECE_LIST *create_piece_list(TABLE *table){
	PIECE_LIST *list = NULL;
	if(table != NULL){
		// Aloca a memoria necessaria
		list = (PIECE_LIST*)malloc(sizeof(PIECE_LIST));
		if(list != NULL){
			int i, j, side_turn;
			side_turn = (table->turn == WHITES_TURN ? WHITES_SIDE : BLACKS_SIDE);

			list->first = NULL;
			list->last = NULL;
			// Percore o tabuleiro
			for(i = 0; i < 8; i++){
				for(j = 0; j < 8; j++){
					// Se existir uma peça e for o turno dela atualmente
					if(table->grid[i][j] != NULL && table->grid[i][j]->side == side_turn){
						// Insere a peça na lista de maneira ordenada
						insert_list(list, table->grid[i][j]);
					}
				}
			}
		}
	}
	return list;
}

int delete_list(PIECE_LIST **list){
	if(list != NULL && *list != NULL){
		AUX_NODE *aux;
		while((*list)->first->next != (*list)->first){
			aux = (*list)->first->next;
			(*list)->first->next = aux->next;
			free(aux);
		}
		free((*list)->first);
		free(*list);
		(*list) = NULL;
		return 0;
	}
	return 1;
}

int print_list(PIECE_LIST *list){
	if(list != NULL){
		AUX_NODE *print = list->first;
		if(print != NULL) printf("||%c||(%d,%c)", print->piece->name, print->piece->rank, print->piece->file);
		print = print->next;
		while(print != list->first){
			printf("->||%c||(%d,%c)", print->piece->name, print->piece->rank, print->piece->file);
			print = print->next;
		}
		printf("\n");
		return 0;
	}
	return 1;
}

void delete_piece(PIECE **piece){
	if(piece != NULL && *piece != NULL){
		free(*piece);
		(*piece) = NULL;
	}
}

TABLE *create_table(void){
	int i, j;
	int error = 0;

	TABLE *table = (TABLE*)malloc(sizeof(TABLE));
	if(table == NULL) error = 1;
	else{
		// Aloca as colunas da grid
		table->grid = (PIECE***)malloc(sizeof(PIECE**) * 8);
		// Caso falhe em alocar a memória, muda o indicador de erro para 1
		if(table == NULL) error = 1;
		// Caso contrário prossegue com a execução
		else{
			// Cria todas as linhas da grid
			for(i = 0; i < 8; i++){
				table->grid[i] = (PIECE**)malloc(sizeof(PIECE*) * 8);
				// Caso a memoria seja alocada corretamente
				if(table->grid[i] != NULL)
					// Inicializa todas as posições como NULL
					for(j = 0; j < 8; j++) table->grid[i][j] = NULL;
				// Caso haja algum erro de alocação de memória
				else{
					// Indica o erro
					error = 1;
					// Libera toda a memória já alocada
					for(j = i-1; j >= 0; j--) free(table->grid[j]);
					free(table->grid);
					free(table);
					// Muda o valor de retorno para null e garante que vai sair do loop atual
					table = NULL;
					i = 9;
				}
			}
			// Caso a grid tenha sido devidamente alocada, inicializa os outros campos
			if(!error){
				table->turn = ' ';
				table->castling = NULL;
				table->en_passant = NULL;
				table->half_turns = -1;
				table->turn = 0;
				table->fen = NULL;
			}
		}
	}
	// Se houver erro, imprime na saída correspondente
	if(error) fprintf(stderr, "create_table(): error allocating memory\n");

	return table;
}

int read_table(FILE *stream, TABLE *table){
	if(table != NULL){
		char *input = my_get_line(stream);
		char *token;
		int i, j, k, cur_side;

		// Obtem as informacoes de cada linha a cada chamada de strtok
		token = strtok(input, DELIMITERS);
		for(i = 0; i < 8; i++){
			for(j = 0, k = 0; token[j] != '\0'; j++){
				// Caso o caractere seja de um número, pula todas as casas vazias indicadas
				if(isdigit(token[j])) k += (token[j] - '0');
				// Caso contrario cria a peca e a insere na posicao atual
				else{
					table->grid[i][k] = create_piece(token[j], (8-i), ('a'+k));
					k++;
				}
			}

			token = strtok(NULL, DELIMITERS);
		}

		// Apos a ultima chamada de strtok, tok armazena a informacao de turno
		table->turn = token[0];
		token = strtok(NULL, DELIMITERS);

		// Copia a informacao de roque
		table->castling = (char*)malloc(sizeof(char) * 5);
		strcpy(table->castling, token);
		token = strtok(NULL, DELIMITERS);

		// Copia a informacao de captura en passant
		table->en_passant = strdup(token);
		token = strtok(NULL, DELIMITERS);

		// Copia o numero de meio turnos
		table->half_turns = atoi(token);
		token = strtok(NULL, DELIMITERS);

		// Copia o numero de turnos passados
		table->cur_turn = atoi(token);

		// Percorre o tabuleiro a procura do rei do turno atual e armazena sua posicao para facilitar checagem de cheque
		cur_side = (table->turn == WHITES_TURN)? WHITES_SIDE : BLACKS_SIDE;
		for(i = 0; i < 8; i++){
			for(j = 0; j < 8; j++){
				if(table->grid[i][j] != NULL && table->grid[i][j]->move == &move_king && table->grid[i][j]->side == cur_side){
					table->turn_king = table->grid[i][j];
					i = 8;
					j = 8;
				}
			}
		}

		// Atualiza a notacao fen do tabuleiro coma a situacao atual
		update_fen(table);
		free(input);
		return 0;
	}

	fprintf(stderr, "read_table(): table is NULL\n");
	return 1;
}

char *dupe_move(TABLE *table){
	// Volta um turno
	table->turn = (table->turn == WHITES_TURN)? BLACKS_TURN : WHITES_TURN;
	table->half_turns--;
	if(table->turn == BLACKS_TURN) table->cur_turn--;
	char *dupe = (char*)malloc(sizeof(char)*5);
	// Cria um movimento fazendo o rei se mover de uma casa para a mesma
	sprintf(dupe, "%c%d%c%d", table->turn_king->file, table->turn_king->rank, table->turn_king->file, table->turn_king->rank);
	// Retorna a string do movimento
	return dupe;
}

// Funcao que checa se um movimento é valido
int is_valid_movement(char orig_file, int orig_rank, char dest_file, int dest_rank, TABLE *table){
	// Se o movimento for dentro dos limites do tabuleiro e fizer uma peca valida se mover
	if(!(orig_file < 'a' || orig_file > 'h' || orig_rank < 1 || orig_rank > 8) &&
	!(dest_file < 'a' || dest_file > 'h' || dest_rank < 1 || dest_rank > 8) &&
	(table->grid[8-orig_rank][orig_file-'a'] != NULL)){
		PIECE *origin = table->grid[8-orig_rank][orig_file-'a'];
		PIECE *destiny = table->grid[8-dest_rank][dest_file-'a'];
		PIECE_LIST *list;
		QUEUE *queue;
		CHESS_MOVE *movement;
		// Se a jogada nao tentar colocar uma peca na mesma posicao de uma peca aliada
		if(!(destiny != NULL && origin->side == destiny->side)){
			// Cria uma lista vazia e insere apenas a peca sendo movida
			list = create_empty_list();
			insert_list(list, origin);
			queue = create_queue();
			// Lista todos os movimentos possiveis dessa peca
			list_moves(table, queue, list);
			// Cria um movimento com as informacoes passadas
			movement = create_move(0, orig_rank, orig_file, dest_rank, dest_file, 0, 0);
			// Esvazia a fila um por um
			while(!empty_queue(queue)){
				// Se o movimento encontrado for igual ao que se deseja realizar
				if(compare_moves(front_queue(queue), movement) == 3){
					// Libera memoria alocada e retorna 1
					delete_queue(&queue);
					delete_list(&list);
					delete_move(&movement);
					return 1;
				}
				dequeue(queue);
			}
			// Caso o movimento nao seja encontrado, libera memoria alocada e retorna 0
			delete_queue(&queue);
			delete_list(&list);
			delete_move(&movement);
		}
	}
	return 0;
}

// Funcao que checa a situacao atual do tabuleiro para avaliar se é possivel algum jogador dar cheque mate
int enough_pieces(TABLE *table){
	int i, j, b_counter = 0, n_counter = 0, B_counter = 0, N_counter = 0;
	// Percorre o tabuleiro contando pecas
	for(i = 0; i < 8; i++){
		for(j = 0; j < 8; j++){
			if(table->grid[i][j] != NULL){
				// Se for encontrado um peao, uma torre ou uma rainha, retorna 1, pois e possivel
				if(table->grid[i][j]->move == move_pawn ||
				   table->grid[i][j]->move == move_rook ||
				   table->grid[i][j]->move == move_queen)
					return 1;
				// Caso nao sejam, vai contando bispos e cavalos
				else{
					if(table->grid[i][j]->name == B_BISHOP) b_counter++;
					if(table->grid[i][j]->name == W_BISHOP) B_counter++;
					if(table->grid[i][j]->name == B_KNIGHT) n_counter++;
					if(table->grid[i][j]->name == W_KNIGHT) N_counter++;
				}
			}
		}
	}
	// Se alguma das condicoes que impossibilitam cheque mate dos dois lados for encontrada retorna 0
	if(b_counter == 0 && B_counter == 0 && N_counter == 0 && n_counter == 0) return 0;
	else{
		if(b_counter == 0 && n_counter == 0){
			if((B_counter != 0 && N_counter == 0) || (B_counter == 0 && N_counter != 0))
				return 0;
		}
		if(B_counter == 0 && N_counter == 0){
			if((b_counter != 0 && n_counter == 0) || (b_counter == 0 && n_counter != 0))
				return 0;
		}
	}

	// Caso contrario retorna 1
	return 1;
}

int move_piece(char *movement, TABLE *table){
	int orig_rank, dest_rank, i, j, mirror, turn_side, dupe, a2;
	char orig_file, dest_file, promotion, search_castling;
	PIECE *piece;
	PIECE_LIST *list;
	QUEUE *queue;

	mirror = (table->turn == WHITES_TURN)? 1 : -1;

	orig_file = movement[0];
	orig_rank = movement[1] - '0';
	dest_file = movement[2];
	dest_rank = movement[3] - '0';
	piece = table->grid[8-orig_rank][orig_file-'a'];
	// Analisa se o movimento sendo realizado é o movimento teste
	dupe = (dest_rank == orig_rank && dest_file == orig_file && piece->move == move_king)? 1: 0;
	if(piece != NULL && piece->move == move_pawn) promotion = movement[4];
	else promotion = '\0';

	if(!dupe && !is_valid_movement(orig_file, orig_rank, dest_file, dest_rank, table)){
		printf("Movimento invalido. Tente novamente.\n");
		return 2;
	}
	// Retira a peça de sua origem
	table->grid[8-orig_rank][orig_file-'a'] = NULL;
	// Analisa se é necessário resetar a contagem de meio-turnos
	if(piece->move == move_pawn || table->grid[8-dest_rank][dest_file-'a'] != NULL) table->half_turns = -1;
	// Apaga a peça capturada, caso seja uma captura
	if(!dupe && table->grid[8-dest_rank][dest_file-'a'] != NULL) delete_piece(&table->grid[8-dest_rank][dest_file-'a']);
	// Apaga a peça capturada, caso seja uma captura en passant
	if(table->en_passant[0] != '-'){
		if(piece->move == move_pawn && dest_file == table->en_passant[0] && dest_rank == table->en_passant[1]-'0')
			table->grid[8-(dest_rank-mirror)][dest_file-'a'] = NULL;
	}
	// Move a peça para o destino
	table->grid[8-dest_rank][dest_file-'a'] = piece;
	// Se a jogada realizada foi um roque, move também a torre correspondente
	if(piece->move == move_king &&  piece->file == 'e'
	   && ((piece->side == WHITES_SIDE && piece->rank == 1) || (piece->side == BLACKS_SIDE && piece->rank == 8))){
	   	// Analisa qual o caracter indicativo do roque sendo realizado
	   	search_castling = '\0';
		if(dest_file == 'c') search_castling = W_QUEEN;
		if(dest_file == 'g') search_castling = W_KING;
		if(piece->side == BLACKS_SIDE) search_castling = tolower(search_castling);
		// Se a jogada realizada não for um possível roque, search_castling valera '\0'
		if(search_castling != '\0'){
			// Busca o caracter indicativo na string do tabuleiro
			for(i = 0; table->castling[i] != '\0'; i++){
				if(table->castling[i] == search_castling){
					// Caso tenha sido encontrado realiza o roque da rainha
					if(dest_file == 'c'){
						table->grid[8-piece->rank][dest_file+1-'a'] = table->grid[8-piece->rank][0];
						table->grid[8-piece->rank][0] = NULL;
						table->grid[8-piece->rank][dest_file+1-'a']->file = dest_file+1;
					// ou do rei
					}else{
						table->grid[8-piece->rank][dest_file-1-'a'] = table->grid[8-piece->rank][7];
						table->grid[8-piece->rank][7] = NULL;
						table->grid[8-piece->rank][dest_file-1-'a']->file = dest_file-1;
					}
				}
			}
		}
	}
	// Atualiza os valores da peça movida em caso de promoção
	piece->rank = dest_rank;
	piece->file = dest_file;
	if(promotion != '\0'){
		piece->name = promotion;
		if(piece->side == BLACKS_SIDE) piece->name = tolower(piece->name);
		switch(promotion){
			case W_ROOK:
				piece->move = &move_rook;
				break;
			case W_KNIGHT:
				piece->move = &move_knight;
				break;
			case W_BISHOP:
				piece->move = &move_bishop;
				break;
			case W_QUEEN:
				piece->move = &move_queen;
				break;
			case W_KING:
				piece->move = &move_king;
				break;
			case W_PAWN:
				piece->move = &move_pawn;
				break;
		}
	}
	// Altera a string de captura en passant se necessário
	free(table->en_passant);
	if(piece->move == move_pawn && dest_rank == (orig_rank + 2*mirror)){
		table->en_passant = (char*)malloc(sizeof(char) * 3);
		sprintf(table->en_passant, "%c%d", dest_file, dest_rank-mirror);
	}else{
		table->en_passant = strdup("-");
	}
	// Altera a string de roque se necessário
	// Se o rei tiver sido movido, apaga todas as possibilidades de roque
	if(!dupe && piece->move == move_king && (dest_file != orig_file || dest_rank != orig_rank)){
		if(piece->side == WHITES_SIDE){
			for(i = 0; table->castling[i] != '\0'; i++){
				if(isupper(table->castling[i])){
					for(j = i; table->castling[j] != '\0'; j++)
						table->castling[j] = table->castling[j+1];
					i--;
				}
			}
		}else{
			for(i = 0; table->castling[i] != '\0'; i++){
				if(islower(table->castling[i])){
					for(j = i; table->castling[j] != '\0'; j++)
						table->castling[j] = table->castling[j+1];
					i--;
				}
			}
		}
		if(table->castling[0] == '\0') table->castling[0] = '-';
	}
	// Analisa os quatro cantos do tabuleiro. Se algum deles não for uma torre do lado esperado,
	// remove o caractere correspondente da string, caso exista
	if(table->grid[0][0] == NULL || !(table->grid[0][0]->move == move_rook && table->grid[0][0]->side == BLACKS_SIDE)){
		for(i = 0; table->castling[i] != '\0'; i++){
			if(table->castling[i] == B_QUEEN){
				for(j = i; table->castling[j] != '\0'; j++)
					table->castling[j] = table->castling[j+1];
				i--;
			}
		}
	}
	if(table->grid[0][7] == NULL || !(table->grid[0][7]->move == move_rook && table->grid[0][7]->side == BLACKS_SIDE)){
		for(i = 0; table->castling[i] != '\0'; i++){
			if(table->castling[i] == B_KING){
				for(j = i; table->castling[j] != '\0'; j++)
					table->castling[j] = table->castling[j+1];
				i--;
			}
		}
	}
	if(table->grid[7][0] == NULL || !(table->grid[7][0]->move == move_rook && table->grid[7][0]->side == WHITES_SIDE)){
		for(i = 0; table->castling[i] != '\0'; i++){
			if(table->castling[i] == W_QUEEN){
				for(j = i; table->castling[j] != '\0'; j++)
					table->castling[j] = table->castling[j+1];
				i--;
			}
		}
	}
	if(table->grid[7][7] == NULL || !(table->grid[7][7]->move == move_rook && table->grid[7][7]->side == WHITES_SIDE)){
		for(i = 0; table->castling[i] != '\0'; i++){
			if(table->castling[i] == W_KING){
				for(j = i; table->castling[j] != '\0'; j++)
					table->castling[j] = table->castling[j+1];
				i--;
			}
		}
	}
	// Altera os valores relativos à contagem de turnos
	table->turn = (table->turn == WHITES_TURN)? BLACKS_TURN : WHITES_TURN;
	table->half_turns++;
	if(table->turn == WHITES_TURN) table->cur_turn++;
	// Busca a posição do rei do jogador atual
	turn_side = (table->turn == WHITES_TURN)? WHITES_SIDE : BLACKS_SIDE;
	for(i = 0; i < 8; i++){
		for(j = 0; j < 8; j++){
			if(table->grid[i][j] != NULL && table->grid[i][j]->move == &move_king && table->grid[i][j]->side == turn_side){
				table->turn_king = table->grid[i][j];
				i = 8;
				j = 8;
			}
		}
	}
	// Checa se o jogador adversário possui alguma jogada possível
	list = create_piece_list(table);
	queue = create_queue();
	list_moves(table, queue, list);
	delete_list(&list);
	update_fen(table);
	if(empty_queue(queue)){
		printf("%s\n", table->fen);
		delete_queue(&queue);
		// Caso não tenha e esteja em cheque, exibe a mensagem de vitória
		if(is_attacked(table, table->turn_king->file, table->turn_king->rank, &a2))
			printf("Xeque-mate -- Vitoria: %s\n", (table->turn == WHITES_TURN)? "PRETO" : "BRANCO");
		// Caso não esteja em cheque, exibe a mensagem de empate por afogamento
		else
			printf("Empate -- Afogamento\n");
		return 0;
	}else delete_queue(&queue);

	// Se o numero de meio turnos for acima do limite, declara empate
	if(table->half_turns >= 50){
		printf("%s\n", table->fen);
		printf("Empate -- Regra dos 50 Movimentos\n");
		return 0;
	}

	// Se nao houver pecas suficientes para cheque mate dos dois lados, declara empate
	if(!enough_pieces(table)){
		printf("%s\n", table->fen);
		printf("Empate -- Falta de Material\n");
		return 0;
	}

	return 1;
}

char *get_fen(TABLE *table){
	if(table != NULL){
		return table->fen;
	}
	return NULL;
}

int update_fen(TABLE *table){
	if(table != NULL){
		int i, j, counter, size = 0;
		char *fen = NULL;

		// Percorre o tabuleiro
		for(i = 0; i < 8; i++){
			// Zera o contador
			counter = 0;
			// Percorre a linha
			for(j = 0; j < 8; j++){
				// Quando encontra uma peca
				if(table->grid[i][j] != NULL){
					// caso o contador seja maior que zero
					if(counter > 0){
						// Aloca a memoria a mais na string e adiciona o numero de espacos vazios
						size++;
						fen = (char*)realloc(fen, sizeof(char) * (size+1));
						sprintf(fen+size-1, "%d", counter);
					}
					// Aloca a memoria a mais e adiciona a peca encontrada
					size++;
					fen = (char*)realloc(fen, sizeof(char) * (size+1));
					sprintf(fen+size-1, "%c", table->grid[i][j]->name);
					// Zera o contador
					counter = 0;
				// Caso contrario incrementa o contador
				} else counter++;
			}
			// Ao chegar no final da fila avalia se devem ser adicionados o numero de espacos vazios no final
			if(counter > 0){
				size++;
				fen = (char*)realloc(fen, sizeof(char) * (size+1));
				sprintf(fen+size-1, "%d", counter);
			}
			// Caso nao seja a ultima linha analisada, adiciona uma divisoria
			if(i < 7){
				size++;
				fen = (char*)realloc(fen, sizeof(char) * (size+1));
				sprintf(fen+size-1, "/");
			}
		}
		// Aumenta o tamanho da string
		size += 8;
		// Analisa se os numeros precisam de um espaco adicional
		if(table->half_turns/10 > 0) size+= 1;
		if(table->cur_turn/10 > 0) size += 1;
		if(table->cur_turn/100 > 0) size+= 1;
		// Aumenta o espaco necessario para as strings de roque e en passant
		size += (int)strlen(table->castling);
		size += (int)strlen(table->en_passant);
		// Aloca a memoria para escrever na string
		fen = (char*)realloc(fen, sizeof(char) * (size+1));
		// Escreve todas as informacoes lidas
		sprintf(fen+strlen(fen), " %c %s %s %d %d", table->turn, table->castling, table->en_passant, table->half_turns, table->cur_turn);
		// Se existia uma fen salva, apaga a anterior
		if(table->fen != NULL) free(table->fen);
		// Salva a nova fen
		table->fen = fen;
		return 0;
	}
	return 1;
}

void list_moves(TABLE *table, QUEUE *queue, PIECE_LIST *list){
	AUX_NODE *node = list->first;
	NODE *start;
	char cur_piece = 0;
	if(node != NULL){
		// Percorre a lista de pecas
		do{
			// Analisa se essa é a primeira peca do seu tipo a ser analisada
			// E salva seu endereco caso seja
			if(cur_piece != node->piece->name) start = queue->first;
			// Enfileira todos os movimentos da peca atual
			node->piece->move(table, queue, node->piece);
			// Avanca para o proximo nó
			node = node->next;
			// Caso o proximo no nao seja mais do mesmo tipo ou seja o ultimo no da lista
			if(cur_piece != node->piece->name || node == list->first){
				// Checa repeticoes de todos os nos com esse mesmo tipo
				if(start == NULL) check_repeats(queue, queue->first);
				else check_repeats(queue, start);
				// Atualiza o tipo de pecas atual
				cur_piece = node->piece->name;
			}
		}while(node != list->first);
	}
}

double piece_score(TABLE *table, PIECE *piece){
	// Retorna o valor da peca, ou 50 quando for casa vazia
	if(piece == NULL) return 50;
	else{
		if(piece->move == move_pawn) return 100;
		if(piece->move == move_knight) return 325;
		if(piece->move == move_bishop) return 325;
		if(piece->move == move_rook) return 550;
		if(piece->move == move_queen) return 1000;
		else return 50000;
	}
}

// Funcao que analisa um movimento e calcula seu valor
double move_score(TABLE *table, CHESS_MOVE *move){
	PIECE *piece = table->grid[8-move->origin_rank][move->origin_file-'a'];
	PIECE *aux = table->grid[8-move->destiny_rank][move->destiny_file-'a'];
	PIECE *rook = NULL;
	int a1, a2, i, j, castling = 0;
	double numerator = 0, denominator = 0, value1, value2;

	// Se o movimento a ser realizado for um roque, move a torre
	if(piece->move == move_king && (move->origin_file - move->destiny_file == 2 || move->origin_file - move->destiny_file == -2)){
		castling = 1;
		if(move->destiny_file == 'c'){
			rook = table->grid[8-piece->rank][0];
			table->grid[8-piece->rank][0] = NULL;
			table->grid[8-piece->rank]['d'-'a'] = rook;
			rook->file = 'd';
		}else{
			rook = table->grid[8-piece->rank][7];
			table->grid[8-piece->rank][7] = NULL;
			table->grid[8-piece->rank]['f'-'a'] = rook;
			rook->file = 'f';
		}
	}

	// Retira a peca da origem
	table->grid[8-move->origin_rank][move->origin_file-'a'] = NULL;
	table->grid[8-move->destiny_rank][move->destiny_file-'a'] = piece;
	// Caso seja uma captura en passant, armazena a peca capturada e retira-a do tabuleiro
	if(move->special == 'E'){
		aux = table->grid[8-piece->rank][move->destiny_file-'a'];
		table->grid[8-piece->rank][move->destiny_file-'a'] = NULL;
	// Caso seja uma promocao, altera o peao que se movimentou
	}else if(move->special != '\0'){
		piece->name = move->special;
		if(move->special == W_KNIGHT) piece->move = move_knight;
		else if(move->special == W_BISHOP) piece->move = move_bishop;
		else if(move->special == W_ROOK) piece->move = move_rook;
		else if(move->special == W_QUEEN) piece->move = move_queen;
		if(piece->side == BLACKS_SIDE) piece->name = tolower(piece->name);
	}
	// Atualiza as informacoes da peca
	piece->file = move->destiny_file;
	piece->rank = move->destiny_rank;
	// Percorre o tabuleiro
	for(i = 0; i < 8; i++){
		for(j = 0; j < 8; j++){
			// Obtem a1, a2, v1 e v2 para todas as caras
			a1 = is_attacked(table, 'a'+j, 8-i, &a2);
			value1 = piece_score(table, table->grid[i][j]);
			value2 = value1;
			// Analisa se v1 e v2 precisa ser divididos por 2
			if(table->grid[i][j] != NULL){
				if(table->grid[i][j]->side == table->turn_king->side) value2 = value2/2.0;
				else value1 = value1/2.0;
			}
			// Incrementa o numerador e o denominador
			numerator += (double)a2*(value2);
			denominator += (double)a1*(value1);
		}
	}
	// Caso a peca tenha sido promovida, faz com que volte a ser um peao
	if(move->special != '\0' && move->special != 'E'){
		piece->name = (piece->side == WHITES_SIDE)? W_PAWN : B_PAWN;
		piece->move = move_pawn;
	}
	// Volta a peca para sua posicao original
	table->grid[8-move->origin_rank][move->origin_file-'a'] = piece;
	piece->rank = move->origin_rank;
	piece->file = move->origin_file;
	table->grid[8-move->destiny_rank][move->destiny_file-'a'] = NULL;
	// Se uma peca foi capturada, retorna-a ao lugar de origem
	if(aux != NULL) table->grid[8-aux->rank][aux->file-'a'] = aux;
	// Se o roque foi realizado, retorna a torre ao lugar de origem
	if(castling){
		if(move->destiny_file == 'c'){
			table->grid[8-piece->rank][0] = rook;
			table->grid[8-piece->rank]['d'-'a'] = NULL;
			rook->file = 'a';
		}else{
			table->grid[8-piece->rank][7] = rook;
			table->grid[8-piece->rank]['f'-'a'] = NULL;
			rook->file = 'h';
		}
	}

	return (numerator/(denominator+1.0));
}

char *ai_move(TABLE *table){
	PIECE_LIST *list = create_piece_list(table);
	QUEUE *queue = create_queue();
	CHESS_MOVE *best_move = NULL;
	double highest_score = -1;
	char *command = (char*)calloc(6, sizeof(char));

	// Lista todos os movimentos que a ia pode realizar
	list_moves(table, queue, list);
	// Avalia todos os movimentos
	while(!empty_queue(queue)){
		// Se o movimento avaliado for melhor que o ultimo melhor, salva ele
		if(move_score(table, front_queue(queue)) > highest_score){
			delete_move(&best_move);
			best_move = copy_move(front_queue(queue));
			highest_score = move_score(table, best_move);
		}
		// Desenfileira
		dequeue(queue);
	}
	// Armazena as informacoes do melhor movimento
	command[0] = best_move->origin_file;
	command[1] = best_move->origin_rank + '0';
	command[2] = best_move->destiny_file;
	command[3] = best_move->destiny_rank + '0';
	command[4] = '\0';
	// Se for uma provocao, armazena para que peca sera promovido
	if(best_move->special != '\0' && best_move->special != 'E') command[4] = best_move->special;
	command[5] = '\0';
	// Libera a memoria alocada
	delete_move(&best_move);
	delete_list(&list);
	delete_queue(&queue);

	// Retorna o melhor comando
	return command;
}

int delete_table(TABLE **table){
	if(table != NULL && *table != NULL){
		int i, j;
		// Apaga todas as pecas e a matriz alocada
		if((*table)->grid != NULL){
			for(i = 0; i < 8; i++){
				for(j = 0; j < 8; j++){
					if((*table)->grid[i][j] != NULL)
						delete_piece(&(*table)->grid[i][j]);
				}
				free((*table)->grid[i]);
			}
			free((*table)->grid);
		}

		// Libera a memoria alocada por strings
		if((*table)->castling != NULL) free((*table)->castling);
		if((*table)->en_passant != NULL) free((*table)->en_passant);
		if((*table)->fen != NULL) free((*table)->fen);

		// Libera a memoria restante e seta NULL no ponteiro
		free(*table);
		(*table) = NULL;
		return 0;
	}
	return 1;
}

int print_table(TABLE *table){
	if(table != NULL){
		int i, j;
		if(table->grid != NULL){
			for(i = 0; i < 8; i++){
				printf("%d|  ", 8-i);
				for(j = 0; j < 8; j++){
					if(table->grid[i][j] != NULL) printf("%c  ", table->grid[i][j]->name);
					else printf("-  ");
				}
				printf("\n");
			}
			printf("    _  _  _  _  _  _  _  _\n");
			printf("    a  b  c  d  e  f  g  h\n");
		}
		printf("\n\n");

		printf("turn: |%c|\n", table->turn);
		if(table->castling != NULL) printf("castling: |%s|\n", table->castling);
		if(table->en_passant != NULL) printf("en_passant: |%s|\n", table->en_passant);
		printf("half-turns: %d\n", table->half_turns);
		printf("current turn: %d\n", table->cur_turn);
		printf("turn king: side = %d; rank = %d; file = %c\n", table->turn_king->side, table->turn_king->rank, table->turn_king->file);

		return 0;
	}
	return 1;
}
