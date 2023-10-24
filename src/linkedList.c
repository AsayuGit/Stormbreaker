#include "linkedList.h"

#include <stdlib.h>
#include <string.h>

void insertLinkedList(LinkedList** list, char* key, char* data) {
    if (!list || !key || !data) return;
    LinkedList* newList = (LinkedList*)calloc(1, sizeof(LinkedList));
    
    newList->key = (char*)malloc((strlen(key) + 1) * sizeof(char));
    strcpy(newList->key, key);
    
    newList->data = (char*)malloc((strlen(data) + 1) * sizeof(char));
    strcpy(newList->data, data);

    if (!(*list)) {
        (*list) = newList;
    } else {
        LinkedList* insList = (*list);
        while (insList->next) insList = insList->next;
        insList->next = newList;
    }
}

char* getLinkedList(LinkedList* list, char* key) {
    if (!list) return NULL;
    if (strcmp(key, list->key)) return getLinkedList(list->next, key);
    return list->data;
}

void freeLinkedList(LinkedList* list) {
    if (!list) return;
    freeLinkedList(list->next);
    free(list);
}