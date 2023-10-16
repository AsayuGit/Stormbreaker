#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

typedef struct LinkedList LinkedList;
struct LinkedList {
    char* key;
    char* data;
    struct LinkedList* next;
};

void insertLinkedList(LinkedList** list, char* key, char* data);
char* getLinkedList(LinkedList* list, char* key);
void freeLinkedList(LinkedList* list);

#endif