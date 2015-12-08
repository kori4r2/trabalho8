//  Ítalo Tobler Silva - nUSP 8551910

#include "hash_table.h"
#include "math.h"

typedef struct hash_element{
	char *string;
	int counter;
}HASH_ELEMENT;

struct hash_table{
	struct hash_element **table;
	int capacity;
	int used;
};

// Funcao que retorna se um numero maior que 3 é primo
int is_prime(int value){
	int i;
	for(i = 3; i <= sqrt(value); i++){
		if(value % i == 0) return 0;
	}
	return 1;
}

// Funcao que retorna o proximo tamanho da hash table
int next_size(int value){
	int next = value;
	// Busca pelo proximo numero primo com modulo 4 igual a 3
	while((next += 2) % 4 != 3 || !is_prime(next));
	// Retorna o valor encontrado
	return next;
}

// Funcao que retorna a posicao onde a string passada deve ser inserida
int get_position(char *string, HASH_TABLE *hash){
	if(string == NULL || hash == NULL) return -1;

	int i = 0, j = 0;
	long int start, end;
	double a = 2;
	double key = 0;

	// Obtem a chave da string a partir da acumulacao polinomial
	while(string[i++] != '\0'){
		key += (double)string[i]*pow(a, j);
		j++;
	}
	// Obtem a primeira posicao onde a string deve ser inserida
	start = ((long int)key)%(hash->capacity);
	end = start;
	i = 0;
	// Enquanto a posicao estiver ocupada por uma string diferente da inserida e nao tiver checado todas as posicoes
	while((hash->table[end] != NULL && strcmp(string, hash->table[end]->string) != 0) && i < hash->capacity){
		// Incrementa o contador i
		i++;
		// Caso i seja impar, soma com o quadrado do contador e obtem a proxima posicao com mod
		if(i%2 == 1) end = (start + (i*i)) % hash->capacity;
		// Caso contrario
		else{
			// Faz a subtracao com o quadrado
			end = (start - (i*i));
			// Se resultar em um numero negativo o calculo do mod deve ser adaptado
			if(end < 0){
				end = end*(-1);
				end = (hash->capacity - (end % hash->capacity)) % hash->capacity;
			// Caso contrario, faz a operacao normalmente
			}else end = end % hash->capacity;
		}
	}
	// Se houver checado todas as posicoes (nunca deve acontecer) exibe mensagem de erro
	if(i == hash->capacity) fprintf(stderr, "checked everything\n");

	// Retorna a posicao final
	return (int)end;
}

// Funcao que cria um elemento de hash a partir de uma string
HASH_ELEMENT *create_element(char *string){
	// Aloca memoria necessaria
	HASH_ELEMENT *element = (HASH_ELEMENT*)malloc(sizeof(HASH_ELEMENT));
	if(element != NULL){
		// Inicializa os valores
		element->string = string;
		element->counter = 1;
	}
	return element;
}

// Funcao que apaga um elememento de hash, liberando memoria alocada e setando NULL no ponteiro
void delete_element(HASH_ELEMENT **element){
	if(element != NULL && *element != NULL){
		// Se necessario, libera a memoria alocada pela string
		if((*element)->string != NULL) free((*element)->string);
		free(*element);
		*element = NULL;
	}
}

HASH_TABLE *create_hash_table(int size){
	// Aloca a memoria
	HASH_TABLE *hash = (HASH_TABLE*)malloc(sizeof(HASH_TABLE));
	if(hash != NULL){
		// Inicializa os valores. Todos os elementos do vetor dever começar como NULL
		hash->table = (HASH_ELEMENT**)calloc(size, sizeof(HASH_ELEMENT*));
		hash->capacity = size;
		hash->used = 0;
	}
	return hash;
}

// Funcao que expande a hash table
HASH_TABLE *expand_table(HASH_TABLE *hash){
	HASH_TABLE *new_hash = NULL;
	if(hash != NULL){
		int i, j;
		// Cria a nova hash table com tamanho maior
		new_hash = create_hash_table(next_size(hash->capacity));
		// Percorre todos os elementos da hash table ja existente
		for(i = 0; i < hash->capacity; i++){
			// Se houver um elemento
			if(hash->table[i] != NULL){
				// Obtem a posicao desse elemento na nova hash
				j = get_position(hash->table[i]->string, new_hash);
				if(j > -1){
					// Se for uma posicao valida, copia para a nova table
					new_hash->table[j] = hash->table[i];
					new_hash->used++;
				}else{
					// Caso contrario, exibe mensagem de erro
					fprintf(stderr, "error copying hash table\n");
					// E apaga o item para evitar memory leak
					delete_element(&hash->table[i]);
				}
			}
		}
		// Libera a memoria alocada pela tabela antiga
		free(hash->table);
		free(hash);
	}
	// Retorna a nova tabela
	return new_hash;
}

void print_hash(HASH_TABLE *hash){
	if(hash != NULL){
		int i;
		for(i = 0; i < hash->capacity; i++){
			if(hash->table[i] != NULL) fprintf(stderr, "%d - %s\n", i, hash->table[i]->string);
		}
	}
}

int insert_hash(HASH_TABLE **hash, char *string){
	// Se algum parametro invalido for passado, retorna -1
	if(hash == NULL || *hash == NULL || string == NULL) return -1;

	int position;

	// Se 75% da tabela tiver sido ocupada, expande a tabela hash
	if(((*hash)->used/1.0) > (0.75*(*hash)->capacity)) *hash = expand_table(*hash);

	// Obtem a posicao do novo elemento
	position = get_position(string, *hash);

	// Caso seja uma posição vazia
	if((*hash)->table[position] == NULL){
		// Cria o novo elemento e incrementa o indicador de espaço
		(*hash)->table[position] = create_element(string);
		(*hash)->used++;
		// Retorna 1 (numero de vezes que essa string foi inserida)
		return 1;
	}else{
		// Caso a posicao atual tenha a string inserida
		if(strcmp(string, (*hash)->table[position]->string) == 0){
			// Libera a string
			free(string);
			// Incrementa o contador e retorna seu novo valor
			(*hash)->table[position]->counter++;
			return (*hash)->table[position]->counter;
		}else{
			// Caso contrario, houve erro na insercao, libera a memoria e retorna 100
			fprintf(stderr, "error inserting %s to hash table, %s existed\n", string, (*hash)->table[position]->string);
			free(string);
			return 100;
		}
	}
}

int delete_hash_table(HASH_TABLE **hash){
	if(hash != NULL && *hash != NULL){
		int i;
		for(i = 0; i < (*hash)->capacity; i++){
			delete_element(&((*hash)->table[i]));
		}
		free((*hash)->table);
		free(*hash);
		*hash = NULL;
		return 0;
	}
	return 1;
}
