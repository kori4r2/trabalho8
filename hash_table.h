#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST_SIZE 256

typedef struct hash_table HASH_TABLE;

HASH_TABLE *create_hash_table(int);
int insert_hash(HASH_TABLE**, char*);
int delete_hash_table(HASH_TABLE**);

#endif
