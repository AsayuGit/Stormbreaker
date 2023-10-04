#include "filemanager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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