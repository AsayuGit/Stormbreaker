#include "linkedList.h"

#include <stdlib.h>
#include <string.h>

void insertLinkedList(LinkedList** list, char* key, char* data) {
    if (!list || !key || !data) return;
    LinkedList* newList = (LinkedList*)calloc(1, sizeof(LinkedList));

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
    return key;
}

void freeLinkedList(LinkedList* list) {
    if (!list) return;
    freeLinkedList(list->next);
    free(list);
}