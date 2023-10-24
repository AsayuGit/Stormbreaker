#ifndef _FILEMANAGER_H
#define _FILEMANAGER_H

#include <stddef.h>
#include <stdio.h>

struct wordlist {
    char** words;
    size_t length;
};

int fetchLine(FILE* file, char* buffer, size_t buffLen);
size_t getLineCount(FILE* file);
void strToLower(char* cstring);

struct wordlist loadWordlist(const char* path);

void freeWordlist(struct wordlist wordlist);

#endif