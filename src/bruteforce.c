#include "bruteforce.h"
#include "hash.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>

#define BUFF_SIZE 256

struct wordlistBFArgs {
    const char* wordlistPath;
    unsigned char targetDigest[32];
};

static void* bfThread(void* args) {
    FILE* wordlist = fopen(((struct wordlistBFArgs*)args)->wordlistPath, "r");

    char buffer[BUFF_SIZE];
    unsigned char digest[32] = {0};

    bool passwordFound = false;
    while (!passwordFound && fgets(buffer, BUFF_SIZE, wordlist)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newlines;
        hash(digest, buffer);

        passwordFound = true;
        for (unsigned int index = 0; index < 32; ++index) {
            if (digest[index] != ((struct wordlistBFArgs*)args)->targetDigest[index]) {
                passwordFound = false;
                break;
            }
        }
    }

    fclose(wordlist);

    char* returnPassword = NULL;
    if (passwordFound) {
        size_t buffLen = strlen(buffer) + 1;
        returnPassword = (char*)malloc(buffLen * sizeof(char));
        memcpy(returnPassword, buffer, buffLen);
    }

    return returnPassword;
}

char* wordlistBF(const char* wordlistPath, const unsigned char targetDigest[32]) {
    pthread_t thread;

    struct wordlistBFArgs* args = calloc(1, sizeof(struct wordlistBFArgs));
    
    args->wordlistPath = wordlistPath;
    memcpy(args->targetDigest, targetDigest, 32);

    pthread_create(&thread, NULL, bfThread, args);

    char* password;
    pthread_join(thread, (void**)&password);

    free(args);
    return password;
}