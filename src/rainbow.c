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

int createRainbow(FILE* input, FILE* output, const char* algorithm, unsigned int nbOfThreads) {
    if (!input || !output) return -1;
    if (nbOfThreads == 0) nbOfThreads = get_nprocs();

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

    return 0;
}

HashTable* loadRainbow(FILE* input) {
    if (!input) return NULL;

    HashTable* table = createHashTable(getLineCount(input));
    if (!table) {
        fprintf(stderr, "ERROR: Unable to create hashtable !\n");
        return NULL;
    }

    printf("Loading Table...\n");
    char buffer[BUFF_LEN];
    while (fetchLine(input, buffer, BUFF_LEN) != EOF) {
        char* hash = strtok(buffer, ":");
        char* password = strtok(NULL, ":");

        insertHashTable(table, hash, password);
    }
    printf("Table Loaded !\n");

    return table;
}

int solveRainbow(HashTable* table, FILE* input, FILE* output, unsigned int nbOfThreads) {
    if (!table || !input || !output) return -1;
    if (nbOfThreads == 0) nbOfThreads = get_nprocs();

    char key[BUFF_LEN];
    char* data;
    while (fetchLine(input, key, BUFF_LEN) != EOF) {
        strToLower(key);
        if ((data = getHashTable(table, key))) {
            printf("MATCH %s %s\n", key, data);
        }
    }

    return 0;
}