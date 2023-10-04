#include "bruteforce.h"
#include "hash.h"
#include "filemanager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include <sys/sysinfo.h>

struct wordlistBFArgs {
    struct wordlist wordlist;
    unsigned char targetDigest[32];
    unsigned int threadIndex;
    unsigned int threadCount;
    bool* passwordFound;
};

static void* bfThread(void* args) {
    unsigned char digest[32] = {0};

    struct wordlistBFArgs* bfArgs = (struct wordlistBFArgs*)args;

    unsigned int segmentLength = bfArgs->wordlist.length / bfArgs->threadCount;
    unsigned int start = segmentLength * bfArgs->threadIndex;
    unsigned int end = start + segmentLength;

    if (bfArgs->threadIndex == bfArgs->threadCount - 1) {
        end += (bfArgs->wordlist.length % bfArgs->threadCount);
    }

    printf("Thread %d scanning range %d %d from %ld\n", bfArgs->threadIndex, start, end, bfArgs->wordlist.length);

    char* buffer = NULL;
    bool passwordFound = false;
    for (unsigned int wordIndex = start; !passwordFound && !(*bfArgs->passwordFound) && (wordIndex < end); ++wordIndex) {
        buffer = bfArgs->wordlist.words[wordIndex];
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
        printf("Thread %d found pass\n", bfArgs->threadIndex);
        (*bfArgs->passwordFound) = true;

        size_t buffLen = strlen(buffer) + 1;
        returnPassword = (char*)malloc(buffLen * sizeof(char));
        memcpy(returnPassword, buffer, buffLen);
    }

    free(args);

    return returnPassword;
}

char* wordlistBF(const char* wordlistPath, const unsigned char targetDigest[32]) {
    int nbOfThreads = get_nprocs();
    pthread_t* threads = (pthread_t*)calloc(nbOfThreads, sizeof(pthread_t));

    bool passwordFound = false;

    struct wordlist wordlist = loadWordlist(wordlistPath);

    for (unsigned int threadIndex = 0; threadIndex < nbOfThreads; ++threadIndex) {
        struct wordlistBFArgs* args = calloc(1, sizeof(struct wordlistBFArgs));

        args->wordlist = wordlist;
        args->threadCount = nbOfThreads;
        args->threadIndex = threadIndex;
        args->passwordFound = &passwordFound; // Pass pointer to boolean to signal if threads found the password

        memcpy(args->targetDigest, targetDigest, 32);
        printf("Starting thread %d\n", threadIndex);
        pthread_create(&threads[threadIndex], NULL, bfThread, args);
    }

    char* password = NULL;
    for (unsigned int threadIndex = 0; threadIndex < nbOfThreads; ++threadIndex) {
        char* buffer = NULL;
        pthread_join(threads[threadIndex], (void**)&buffer);
        if (buffer) password = buffer;
    }

    freeWordlist(wordlist);

    return password;
}