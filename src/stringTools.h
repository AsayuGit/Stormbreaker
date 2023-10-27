#ifndef _STRINGTOOLS_H
#define _STRINGTOOLS_H

#include <stddef.h>
#include <stdio.h>

int fetchLine(FILE* file, char* buffer, size_t buffLen);
size_t getLineCount(FILE* file);
void strToLower(char* cstring);

#endif