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

/*------------------------------------------------------------------------------------------------------------
   create_piece_list()
        Funcao que cria uma lista com todas as pecas do jogador atual, ordenando-as de acordo com sua pontuacao
        - Parametros
          TABLE* : ponteiro para o tabuleiro
        - Retorno
          PIECE_LIST* : lista de pecas, alocada na memoria
*/
PIECE_LIST *create_piece_list(TABLE*);

/*------------------------------------------------------------------------------------------------------------
   delete_list()
        Funcao que apaga uma lista de pecas, liberando a memoria alocada, mas nao apagando as pecas em si
        - Parametros
          PIECE_LIST* : ponteiro para a lista de pecas
        - Retorno
          int : 1 - parametro invalido passado; 0 - operacao bem-sucedida;
*/
int delete_list(PIECE_LIST**);

/*------------------------------------------------------------------------------------------------------------
   print_list()
        Funcao auxiliar que imprime a lista de pecas na tela para facilitar o processo de debug
        - Parametros
          PIECE_LIST* : ponteiro para a lista de pecas
        - Retorno
          int : 1 - parametro invalido passado; 0 - operacao bem-sucedida;
*/
int print_list(PIECE_LIST*);

/*------------------------------------------------------------------------------------------------------------
   list_moves()
        Funcao que percorre uma lista de pecas e enfileira todos as jogadas que essas pecas podem realizar em
	uma fila
        - Parametros
          TABLE* : ponteiro para o tabuleiro
          QUEUE* : ponteiro para a fila de movimentos
          PIECE_LIST* : ponteiro para a lista de pecas
        - Retorno
          void
*/
void list_moves(TABLE*, QUEUE*, PIECE_LIST*);

/*------------------------------------------------------------------------------------------------------------
   ai_move()
        Funcao que analisa um tabuleiro e retorna a notacao algebrica simplicada da melhor jogada a ser feita
        - Parametros
          TABLE* : ponteiro para o tabuleiro
        - Retorno
          char* : string contendo a jogada analisada
*/
char *ai_move(TABLE*);

/*------------------------------------------------------------------------------------------------------------
   create_table()
        Funcao que cria um tabuleiro, alocando a memoria necessaria
        - Parametros
          void
        - Retorno
          TABLE* : ponteiro para o tabuleiro; NULL caso erro de alocacao de memoria
*/
TABLE *create_table(void);

/*------------------------------------------------------------------------------------------------------------
   read_table()
        Funcao que le a notacao fen de um tabuleiro a partir de uma stream de dados passada e armazena as
	as informacoes lidas
        - Parametros
          FILE* : stream de dados a ser lida
          TABLE* : ponteiro para o tabuleiro a ser atualizado
        - Retorno
          int : 1 - parametro invalido passado; 0 - operacao bem-sucedida
*/
int read_table(FILE*, TABLE*);

/*------------------------------------------------------------------------------------------------------------
   move_piece()
        Funcao que analisa se um movimento passado é valido, avisando caso nao seja e realizando-o caso o seja,
	analisando em seguida se o movimento resultou em fim de jogo, avisando e imprimindo o estado atual do
	tabuleio quando isso acontecer
        - Parametros
          char* : string contendo a notacao algebrica simplificada do movimento
          TABLE* : ponteiro para o tabuleiro
        - Retorno
          int : 2 - movimento invalido; 1 - movimento valido, o jogo continua; 0 - movimento valido, fim de jogo
*/
int move_piece(char*, TABLE*);

/*------------------------------------------------------------------------------------------------------------
   get_fen()
        Funcao que retorna a string contendo a notacao fen atual do tabuleiro. A string em si ja esta alocada,
	se o retorno da funcao for liberado com um free, pode causar problemas
        - Parametros
          TABLE* : ponteiro para o tabuleiro
        - Retorno
          char* : string contendo a notacao fen atual do tabuleiro; NULL caso parametro invalido passado
*/
char *get_fen(TABLE*);

/*------------------------------------------------------------------------------------------------------------
   dupe_move()
        Funcao que retorna um movimento inutil, movendo o rei da rodada anterior para a mesma casa de origem,
	cuja funcao é analisar a situacao inicial do tabuleiro. Esse movimento é identificado em move_piece,
	o que evita alterar a string de roque na fen do tabuleiro
        - Parametros
          TABLE* : ponteiro para o tabuleiro
        - Retorno
          char* : string contendo o movimento para teste
*/
char *dupe_move(TABLE*);

/*------------------------------------------------------------------------------------------------------------
   cur_turn()
        Funcao que retorna de quem é o turno atual no tabuleiro
        - Parametros
          TABLE* : ponteiro para o tabuleiro
        - Retorno
          char : caractere indicador de turno; \0 caso parametro invalido passado
*/
char cur_turn(TABLE*);

/*------------------------------------------------------------------------------------------------------------
   is_attacked()
        Funcao que analisa se a posicao passada esta sendo atacada por pecas aliadas e/ou inimigas
        - Parametros
          TABLE* : ponteiro para o tabuleiro
          char : coluna da casa a ser analisada
          int : fila da casa a ser analisada
          int* : endereco da variavel que armazena 1 caso a posicao seja atacada por peca aliada e 0 caso contrario
        - Retorno
          int : 1 - a posicao esta sendo atacada por uma peca inimiga; 0 - a posicao nao esta sendo atacada
*/
int is_attacked(TABLE*, char, int, int*);

/*------------------------------------------------------------------------------------------------------------
   delete_table()
        Funcao que apaga o tabuleiro, liberando toda a memoria alocada e setando NULL no ponteiro
        - Parametros
          TABLE** : endereco do ponteiro para o tabuleiro
        - Retorno
          int : 1 - parametro invalido passado; 0 - operacao bem-sucedida;
*/
int delete_table(TABLE**);

/*------------------------------------------------------------------------------------------------------------
   print_table()
        Funcao auxiliar que imprime o tabuleiro de ma maneira a facilitar sua visualizacao e com todas as informacoes
	visiveis. (funcao de debug)
        - Parametros
          TABLE* : ponteiro para o tabuleiro
        - Retorno
          int : 1 - parametro invalido passado; 0 - operacao bem-sucedida;
*/
int print_table(TABLE*);

#endif
