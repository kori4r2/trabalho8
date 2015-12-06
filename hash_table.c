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

int get_position(char *string, HASH_TABLE *hash){
	if(string == NULL || hash == NULL) return -1;

	int i = 0, j = 1;
	int start, end;
	double a = 2;
	double position = 0;

	while(string[i++] != '\0'){
		position += pow(a, (double)j);
		j++;
	}
	start = (int)position%hash->capacity;
	end = start;
	i = 1;
	while(hash->table[end] != NULL && strcmp(string, hash->table[end]->string) != 0 && i < hash->capacity){
		end = start + ((int)pow(i, 2) % hash->capacity);
		i++;
	}

	return end;
}

HASH_ELEMENT *create_element(char *string){
	HASH_ELEMENT *element = (HASH_ELEMENT*)malloc(sizeof(HASH_ELEMENT));
	if(element != NULL){
		element->string = string;
		element->counter = 1;
	}
	return element;
}

void delete_element(HASH_ELEMENT **element){
	if(element != NULL && *element != NULL){
		if((*element)->string != NULL) free((*element)->string);
		free(*element);
		*element = NULL;
	}
}

HASH_TABLE *create_hash_table(int size){
	HASH_TABLE *hash = (HASH_TABLE*)malloc(sizeof(HASH_TABLE));
	if(hash != NULL){
		hash->table = (HASH_ELEMENT**)calloc(size, sizeof(HASH_ELEMENT*));
		hash->capacity = size;
		hash->used = 0;
	}
	return hash;
}

HASH_TABLE *expand_table(HASH_TABLE *hash){
	HASH_TABLE *new_hash = NULL;
	if(hash != NULL){
		int i, j;
		hash = create_hash_table(hash->capacity*2);
		for(i = 0; i < hash->capacity; i++){
			if(hash->table[i] != NULL){
				j = get_position(hash->table[i]->string, new_hash);
				if(j > -1) new_hash->table[j] = hash->table[i];
				else{
					fprintf(stderr, "error copying hash table\n");
					delete_element(&hash->table[i]);
				}
			}
		}
		free(hash->table);
		free(hash);
	}
	return new_hash;
}

int insert_hash(HASH_TABLE **hash, char *string){
	if(hash == NULL || *hash == NULL || string == NULL) return -1;

	int position;

	if(((*hash)->used/1.0) > (0.8*(*hash)->capacity)) *hash = expand_table(*hash);
	position = get_position(string, *hash);

	if((*hash)->table[position] == NULL){
		(*hash)->table[position] = create_element(string);
		(*hash)->used++;
		return 1;
	}else{
		if(strcmp(string, (*hash)->table[position]->string) == 0){
			free(string);
			(*hash)->table[position]->counter++;
			return (*hash)->table[position]->counter;
		}else{
			free(string);
			fprintf(stderr, "error inserting to hash table\n");
			return -1;
		}
	}
}

int delete_hash_table(HASH_TABLE **hash){
	if(hash != NULL && *hash != NULL){
		int i;
		for(i = 0; i < (*hash)->capacity; i++){
			delete_element(&((*hash)->table[i]));
		}
		free(*hash);
		*hash = NULL;
		return 0;
	}
	return 1;
}