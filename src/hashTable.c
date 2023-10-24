#include "hashTable.h"

#include "linkedList.h"
#include "hash.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct HashTable {
    LinkedList** array;
    size_t size;
};

HashTable* createHashTable(size_t size) {
    HashTable* newHashTable = (HashTable*)malloc(sizeof(HashTable));

    newHashTable->array = (LinkedList**)calloc(size, sizeof(LinkedList*));
    newHashTable->size = size;

    return newHashTable;
}

void freeHashTable(HashTable* table) {
    for (unsigned int tableIndex = 0; tableIndex < table->size; ++tableIndex) {
        freeLinkedList(table->array[tableIndex]);
    }
    free(table);
}

void insertHashTable(HashTable* table, char* key, char* data) {
    // Derive the table index
    unsigned int tableIndex = hashIndex(key, strlen(key)) % table->size;

    //printf("Inserting [%s, %s] at %d\n", key, data, tableIndex);

    // Insert the new linked list node
    insertLinkedList(&(table->array[tableIndex]), key, data);
}

char* getHashTable(HashTable* table, char* key) {
    // Derive the table index
    unsigned int tableIndex = hashIndex(key, strlen(key)) % table->size;

    // Return data
    return getLinkedList(table->array[tableIndex], key);
}