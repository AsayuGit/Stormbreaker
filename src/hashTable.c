#include "hashTable.h"

#include "linkedList.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Definition of the hashtable struct
struct HashTable {
    LinkedList** array; // Array of linked list pointers
    size_t size; // Size of the allocated hash table
};

// Initialize a new hashtable of a specified size
HashTable* createHashTable(size_t size) {
    LinkedList** list;
    HashTable* newHashTable;

    // Allocate the hashtable's linkedlist array with the specified size
    // Using calloc to properly initialize each field to zero
    if (!(list = (LinkedList**)calloc(size, sizeof(LinkedList*)))) {
        fprintf(stderr, "FATAL: Unable to allocate memory\n");
        return NULL;
    }

    // Allocating memory for the hash table struct
    // using malloc since each fild will be filled out with a value
    if (!(newHashTable = (HashTable*)malloc(sizeof(HashTable)))) {
        fprintf(stderr, "FATAL: Unable to allocate memory\n");
        free(list); // free since list is an array of null pointers
        return NULL;
    }

    // Assigning the linked list array to the allocated hash table
    newHashTable->array = list;
    newHashTable->size = size;

    // Return the newly allocated hash table
    return newHashTable;
}

// Free a previously allocated hashtable
void freeHashTable(HashTable* table) {
    if (!table) return;

    // For each item in the hash table
    for (unsigned int tableIndex = 0; tableIndex < table->size; ++tableIndex) {
        // Free its embeded linkedlist
        freeLinkedList(table->array[tableIndex]);
    }

    // Then free the struct itself
    free(table);
}

// Derive an index in the hash table from the data
unsigned int hashIndex(const char* data, size_t tableSize) {
    unsigned int index = 0;

    size_t size = strlen(data);
    // Simple hash algorithm to derive a number from a cstring
    for (unsigned int i = 0; i < size; ++i) {
        index = (index ^ data[i]) << 1;
    }

    // Then return the index in the range of the hash table
    return index % tableSize;
}

// Insert an hash and its key into an hashtable
void insertHashTable(HashTable* table, char* key, char* data) {
    // Derive the table index
    unsigned int tableIndex = hashIndex(key, table->size);

    // Insert the new linked list node
    insertLinkedList(&(table->array[tableIndex]), key, data);
}

// Fetch an hash with its key from an hashtable
char* getHashTable(HashTable* table, char* key) {
    // Derive the table index
    unsigned int tableIndex = hashIndex(key, table->size);

    // Return data
    return getLinkedList(table->array[tableIndex], key);
}