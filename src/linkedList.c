#include "linkedList.h"

#include <stdlib.h>
#include <string.h>

// Allow to create / insert new data in the linked list
LinkedList* insertLinkedList(LinkedList** list, char* key, char* data) {
    if (!list) return NULL; // Can't insert without destination

    // We allocate a new list node
    LinkedList* newList = (LinkedList*)calloc(1, sizeof(LinkedList));
    
    // They we copy the data and key over (if available)
    if (key) {
        newList->key = (char*)malloc((strlen(key) + 1) * sizeof(char));
        strcpy(newList->key, key);
    }

    if (data) {
        newList->data = (char*)malloc((strlen(data) + 1) * sizeof(char));
        strcpy(newList->data, data);
    }

    // If the list is empty then we set its pointer to the new list
    if (!(*list)) {
        (*list) = newList;
    } else { // If not then we append this new node at the end of the list
        LinkedList* insList = (*list);
        while (insList->next) insList = insList->next;
        insList->next = newList;
    }

    // Return the newly inserted linked list node
    return newList;
}

// Return the data assotiated to a key if available in the linked list
LinkedList* getLinkedList(LinkedList* list, char* key) {
    if (!list) return NULL; // If no list return
    if (strcmp(key, list->key)) return getLinkedList(list->next, key); // If the key doesn't match keep searching
    return list; // If it does then return the list node
}

// Free a previoulsy allocated linked list and its data
void freeLinkedList(LinkedList* list) {
    if (!list) return; // If no list just return
    freeLinkedList(list->next); // Free the rest of the list recursively
    
    // Free the data and the key
    free(list->data);
    free(list->key);
    
    free(list); // Then free the list itself
}