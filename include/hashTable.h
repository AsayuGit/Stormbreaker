#ifndef _HASHTABLE_H
#define _HASHTABLE_H

// The hash table struct is opaque to force the user to use
// the provided functions
typedef struct HashTable HashTable;

#include <stddef.h>

HashTable* createHashTable(size_t size);
void freeHashTable(HashTable* table);
void insertHashTable(HashTable* table, char* key, char* data);
char* getHashTable(HashTable* table, char* key);

#endif