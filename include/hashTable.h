#ifndef _HASHTABLE_H
#define _HASHTABLE_H

// The hash table struct is opaque to force the user to use
// the provided functions
typedef struct HashTable HashTable;

#include <stddef.h>
#include <stdbool.h>

HashTable* createHashTable(size_t size);
void freeHashTable(HashTable* table);
bool insertHashTable(HashTable* table, char* key, char* data);
bool getHashTable(HashTable* table, char* key, char** data);

#endif