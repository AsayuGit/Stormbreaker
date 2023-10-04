#include "filemanager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 256

struct wordlist loadWordlist(const char* path) {
    FILE* wordlistFile = fopen(path, "r");

    size_t linecount = 0;
    for (char c = getc(wordlistFile); c != EOF; c = getc(wordlistFile)) {
        if (c == '\n') ++linecount;
    }
    fseek(wordlistFile, 0, SEEK_SET); // Go back to begining of the file

    struct wordlist list = {
        (char**)calloc(linecount, sizeof(char*)),
        linecount
    };

    char buffer[BUFF_SIZE] = {0};
    for (unsigned int wordIndex = 0; (wordIndex < linecount) &&  fgets(buffer, BUFF_SIZE, wordlistFile); ++wordIndex) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newlines;
        
        size_t buffLen = strlen(buffer) + 1;
        list.words[wordIndex] = (char*)malloc(buffLen * sizeof(char));
        memcpy(list.words[wordIndex], buffer, buffLen);
    }

    fclose(wordlistFile);

    return list;
}

void freeWordlist(struct wordlist wordlist) {
    for (unsigned int wordIndex = 0; wordIndex < wordlist.length; ++wordIndex) {
        free(wordlist.words[wordIndex]);
    }
    free(wordlist.words);
}