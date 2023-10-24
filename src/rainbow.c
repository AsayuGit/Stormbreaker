#include "rainbow.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#include "hash.h"
#include "filemanager.h"

#define BUFF_LEN 256

struct rainbowArgs {
    FILE* input;
    FILE* output;
    pthread_mutex_t *inputMutex;
    pthread_mutex_t *outputMutex;
};

int safeFetchLine(FILE* file, char* buffer, size_t buffLen, pthread_mutex_t* mutex) {
    int status;
    
    pthread_mutex_lock(mutex);
    status = fetchLine(file, buffer, buffLen);
    pthread_mutex_unlock(mutex);

    return status;
}

void* rainbowThread(void* args) {
    struct rainbowArgs* threadArgs = args;

    char password[BUFF_LEN];
    char reducedPassword[BUFF_LEN];
    unsigned char digest[32];

    // Fetch a password
    while (safeFetchLine(threadArgs->input, password, BUFF_LEN, threadArgs->inputMutex) != EOF) {
        // Compute pass couple

        // 1. hash password
        hash(digest, password);

        // 2. reduce hash
        digestToCString(digest, reducedPassword);

        // write back
        pthread_mutex_lock(threadArgs->outputMutex);
        fprintf(threadArgs->output, "%s:%s\n", reducedPassword, password);
        pthread_mutex_unlock(threadArgs->outputMutex);
    }

    return NULL;
}

void createRainbow(const char* wordlistInPath, const char* rainbowOutPath) {
    createRainbowEx(wordlistInPath, rainbowOutPath, get_nprocs());
}

void createRainbowEx(const char* wordlistInPath, const char* rainbowOutPath, unsigned int nbOfThreads) {
    if (!wordlistInPath || !rainbowOutPath) return;

    FILE* input = fopen(wordlistInPath, "r"); // Open wordlist (r)
    FILE* output = fopen(rainbowOutPath, "w"); // Open rainbow (w)
    if (!input | !output) return;

    pthread_t* threads = (pthread_t*)calloc(nbOfThreads, sizeof(pthread_t));
    pthread_mutex_t inputMutex;
    pthread_mutex_t outputMutex;

    pthread_mutex_init(&inputMutex, NULL);
    pthread_mutex_init(&outputMutex, NULL);

    struct rainbowArgs args = {
        input,
        output,
        &inputMutex,
        &outputMutex
    };

    for (unsigned int threadID = 0; threadID < nbOfThreads; ++threadID) {
        pthread_create(&threads[threadID], NULL, rainbowThread, &args);
    }

    for (unsigned int threadID = 0; threadID < nbOfThreads; ++threadID) {
        pthread_join(threads[threadID], NULL);
    }

    pthread_mutex_destroy(&inputMutex);
    pthread_mutex_destroy(&outputMutex);

    free(threads);

    // Close files
    fclose(output);
    fclose(input);
}

HashTable* loadRainbow(const char* rainbowPath) {
    FILE* input = fopen(rainbowPath, "r");
    if (!input) return NULL;

    HashTable* table = createHashTable(100);

    char buffer[BUFF_LEN];
    while (fetchLine(input, buffer, BUFF_LEN) != EOF) {
        char* hash = strtok(buffer, ":");
        char* password = strtok(NULL, ":");

        printf("Hash : >%s< | Password : >%s<\n", hash, password);

        insertHashTable(table, hash, password);
    }

    fclose(input);
    return table;
}