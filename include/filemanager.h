#ifndef _FILEMANAGER_H
#define _FILEMANAGER_H

#include <stddef.h>

struct wordlist {
    char** words;
    size_t length;
};

struct wordlist loadWordlist(const char* path);

void freeWordlist(struct wordlist wordlist);

#endif