#include "bruteforce.h"
#include "hash.h"
#include "filemanager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>

#define THERAD_COUNT 2

struct wordlistBFArgs {
    struct wordlist wordlist;
    unsigned char targetDigest[32];
    unsigned int threadIndex;
    unsigned int threadCount;
};

static void* bfThread(void* args) {
    unsigned char digest[32] = {0};

    struct wordlistBFArgs* bfArgs = (struct wordlistBFArgs*)args;

    unsigned int segmentLength = bfArgs->wordlist.length / bfArgs->threadCount;
    unsigned int start = segmentLength * bfArgs->threadIndex;
    unsigned int end = start + segmentLength;

    printf("%d %d %d\n", segmentLength, start, end);

    // MMMmm..
    char* buffer = NULL;
    bool passwordFound = false;
    for (unsigned int wordIndex = start; !passwordFound && (wordIndex < end); ++wordIndex) {
        buffer = bfArgs->wordlist.words[wordIndex];
        printf("hash [%d] >%s<\n", bfArgs->threadIndex, buffer);
        hash(digest, buffer);

        passwordFound = true;
        for (unsigned int index = 0; index < 32; ++index) {
            if (digest[index] != ((struct wordlistBFArgs*)args)->targetDigest[index]) {
                passwordFound = false;
                break;
            }
        }
    }

    char* returnPassword = NULL;
    if (passwordFound) {
        size_t buffLen = strlen(buffer) + 1;
        returnPassword = (char*)malloc(buffLen * sizeof(char));
        memcpy(returnPassword, buffer, buffLen);
    }

    freeWordlist(bfArgs->wordlist);
    free(args);

    return returnPassword;
}

char* wordlistBF(const char* wordlistPath, const unsigned char targetDigest[32]) {
    pthread_t* threads = (pthread_t*)calloc(THERAD_COUNT, sizeof(pthread_t));

    for (unsigned int threadIndex = 0; threadIndex < THERAD_COUNT; ++threadIndex) {
        struct wordlistBFArgs* args = calloc(1, sizeof(struct wordlistBFArgs));

        args->wordlist = loadWordlist(wordlistPath);
        args->threadCount = THERAD_COUNT;
        args->threadIndex = threadIndex;

        memcpy(args->targetDigest, targetDigest, 32);
        pthread_create(&threads[threadIndex], NULL, bfThread, args);
    }

    char* password;
    for (unsigned int threadIndex = 0; threadIndex < THERAD_COUNT; ++threadIndex) {
        pthread_join(threads[threadIndex], (void**)&password);
    }

    return password;
}