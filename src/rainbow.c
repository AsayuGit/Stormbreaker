#include "rainbow.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <openssl/evp.h>

#include "stringTools.h"

#define BUFF_LEN 256
#define STEP_RATE 2

struct rainbowArgs {
    const EVP_MD* algorithm;
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
    char base64Password[BUFF_LEN];
    unsigned char digest[EVP_MAX_MD_SIZE];

    // Init
    EVP_MD_CTX* shaContext = EVP_MD_CTX_new();

    // Fetch a password
    while (safeFetchLine(threadArgs->input, password, BUFF_LEN, threadArgs->inputMutex) != EOF) {
        // Compute pass couple

        // 1. hash password
        EVP_DigestInit(shaContext, threadArgs->algorithm);
        EVP_DigestUpdate(shaContext, password, strlen(password) * sizeof(char));
        unsigned int digestSize;
        EVP_DigestFinal(shaContext, digest, &digestSize);

        // 2. reduce hash 
        for (unsigned int index = 0; index < digestSize; ++index) {
            sprintf(base64Password + (index << 1), "%02x", digest[index]);
        }

        // write back
        pthread_mutex_lock(threadArgs->outputMutex);
        fprintf(threadArgs->output, "%s:%s\n", base64Password, password);
        pthread_mutex_unlock(threadArgs->outputMutex);
    }

    EVP_MD_CTX_free(shaContext);

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

    const EVP_MD* algo = EVP_get_digestbyname(algorithm);
    if (!algo) {
        fprintf(stderr, "FATAL: Digest Invalid !\n");
        return -1;
    }

    struct rainbowArgs args = {
        algo,
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

    size_t lineCount = getLineCount(input);
    HashTable* table = createHashTable(lineCount);
    if (!table) {
        fprintf(stderr, "FATAL: Unable to create hashtable !\n");
        return NULL;
    }

    size_t nbOfLoadedHashes = 0;
    size_t step = 10;
    char buffer[BUFF_LEN];
    while (fetchLine(input, buffer, BUFF_LEN) != EOF) {
        char* hash = strtok(buffer, ":");
        char* password = strtok(NULL, ":");

        insertHashTable(table, hash, password);
        ++nbOfLoadedHashes;
        if (nbOfLoadedHashes >= step) {
            printf("INFO: Loading %ld hashes...\n", nbOfLoadedHashes);
            step *= STEP_RATE;
        } 
    }
    printf("INFO: %ld hashs loaded.\n", nbOfLoadedHashes);

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
            fprintf(output, "MATCH %s %s\n", key, data);
        }
    }

    return 0;
}