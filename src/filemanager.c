#include "filemanager.h"

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

struct wordlist loadWordlist(const char* path) {
    FILE* wordlistFile = fopen(path, "r");

    size_t linecount = 0;
    size_t maxLen = 0;
    size_t curLen = 0;
    for (char c = getc(wordlistFile); c != EOF; c = getc(wordlistFile)) {
        if (c == '\n') {
            ++linecount;
            if (curLen > maxLen) maxLen = curLen;
            curLen = 0;
        } else {
            ++curLen;
        }
    }
    fseek(wordlistFile, 0, SEEK_SET); // Go back to begining of the file

    struct wordlist list = {
        (char**)calloc(linecount, sizeof(char*)),
        linecount
    };

    char* buffer = (char*)calloc(maxLen, sizeof(char));
    for (unsigned int wordIndex = 0; wordIndex < linecount; ++wordIndex) {
        unsigned int buffIndex = 0;
        for (char c = fgetc(wordlistFile); c != '\n'; c = fgetc(wordlistFile)) {
            buffer[buffIndex++] = c;
        }
        buffer[buffIndex] = '\0';
        
        size_t buffLen = strlen(buffer) + 1;
        list.words[wordIndex] = (char*)malloc(buffLen * sizeof(char));
        memcpy(list.words[wordIndex], buffer, buffLen);
    }
    
    free(buffer);
    fclose(wordlistFile);

    return list;
}

void freeWordlist(struct wordlist wordlist) {
    for (unsigned int wordIndex = 0; wordIndex < wordlist.length; ++wordIndex) {
        free(wordlist.words[wordIndex]);
    }
    free(wordlist.words);
}