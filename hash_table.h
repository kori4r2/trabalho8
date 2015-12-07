#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// O tamanho inicial da hash está pequeno para que possibilite o uso da funcao de expansao da tabela
#define FIRST_SIZE 23

typedef struct hash_table HASH_TABLE;

/*------------------------------------------------------------------------------------------------------------
   create_hash_table()
        Funcao que cria uma hash table do tamanho passado como parametro
        - Parametros
          int : tamanho da hash table
        - Retorno
          HASH_TABLE* : ponteiro para a hash table criada
*/
HASH_TABLE *create_hash_table(int);

/*------------------------------------------------------------------------------------------------------------
   insert_hash()
        Funcao que insere uma string na hash table
        - Parametros
          HASH_TABLE* : ponteiro para a hash table
        - Retorno
          int : quantas vezes a string passada foi inserida na hash table ; -1 caso parametros invalidos; 100
	  	caso insercao falhe
*/
int insert_hash(HASH_TABLE**, char*);

/*------------------------------------------------------------------------------------------------------------
   delete_hash_table()
        Funcao que apaga uma hash table, liberando a memoria alocada e fazendo com que seu ponteiro seja NULL
        - Parametros
          HASH_TABLE** : endereço do ponteiro para a hash table
        - Retorno
          int : 1 - parametro invalido passado; 0 - operacao bem-sucedida;
*/
int delete_hash_table(HASH_TABLE**);

/*------------------------------------------------------------------------------------------------------------
   print_hash()
        Funcao auxiliar que imprime todas as strings contidas na hash na saida de erro
        - Parametros
          HASH_TABLE* : ponteiro para a hash table
        - Retorno
          void
*/
void print_hash(HASH_TABLE*);

#endif
