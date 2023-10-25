#include "stringTools.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Char by char to get arround "corrupted" strings
int fetchLine(FILE* file, char* buffer, size_t buffLen) {
    if (!buffer) return EOF;
    if (!buffLen) return EOF;
    --buffLen;

    unsigned int buffIndex = 0;
    char c = '\0';
    do {
        c = fgetc(file);
        if (c == '\n') break;
        
        buffer[buffIndex] = c;
        ++buffIndex;
        
        if (buffIndex > buffLen) break;
    } while (c != EOF);

    buffer[buffIndex] = '\0';
    return c;
}

size_t getLineCount(FILE* file) {
    if (!file) return 0;

    size_t linecount = 0;
    for (char c = getc(file); c != EOF; c = getc(file)) {
        if (c == '\n') ++linecount;
    }

    fseek(file, 0, SEEK_SET); // Go back to begining of the file

    return linecount;
}

void strToLower(char* cstring) {
    for (; *cstring != '\0'; ++cstring) {
        *cstring = tolower(*cstring);
    }
}