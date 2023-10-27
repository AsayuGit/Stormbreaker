#include "rainbow.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/sysinfo.h>

#include "outFileBuffer.h"
#include "inFileBuffer.h"

struct SolveThreadArgs {
    InFileBuffer* input;          // Input where to read the hashes from
    OutFileBuffer* output;        // Output where to put the matches to
    HashTable* table;             // The loaded rainbow table
    pthread_mutex_t* inputMutex;  // Read Mutex
    pthread_mutex_t* outputMutex; // Write Mutex
    bool minimalOutput;           // Wether to restrict the output or not
};

// This is the function executed by each threads of solveRainbow()
void* solveThread(void* args) {
    struct SolveThreadArgs* threadArgs = args;

    char key[BUFF_LEN];
    char* data;
    // For each line of the input
    while (safeReadLineInFileBuffer(threadArgs->input, key, BUFF_LEN, threadArgs->inputMutex) != EOF) {
        // Ensure the hash is in lowercase
        for (unsigned int i = 0; key[i] != '\0'; ++i) {
            key[i] = tolower(key[i]);
        }

        // Then try to find it in the hash table
        if (!getHashTable(threadArgs->table, key, &data)) continue;
        
        pthread_mutex_lock(threadArgs->outputMutex);

        // If found, output the match
        if (!threadArgs->minimalOutput) {
            writeOutFileBuffer(threadArgs->output, "MATCH ", 6);
            writeOutFileBuffer(threadArgs->output, key, strlen(key));
            writeOutFileBuffer(threadArgs->output, " ", 1);
        }

        if (data) writeOutFileBuffer(threadArgs->output, data, strlen(data));
        writeOutFileBuffer(threadArgs->output, "\n", 1);

        pthread_mutex_unlock(threadArgs->outputMutex);
    }

    // Then exit one there's no more data to process
    return NULL;
}

// Try to find each hash from the input in the hash table and output matches
int solveRainbow(HashTable* table, FILE* input, FILE* output, unsigned int nbOfThreads, bool minimalOutput) {
    if (!table || !input || !output) return -1;
    if (nbOfThreads == 0) nbOfThreads = get_nprocs(); // 0 means "All available hardware threads"

    // First we allocate the IO buffers
    InFileBuffer* inBuffer = openInFileBuffer(input, IO_BUFFER_SIZE);
    OutFileBuffer* outBuffer = openOutFileBuffer(output, IO_BUFFER_SIZE);

    // Then, we initialize an array of pthread_t* to store the handle of each thread
    // This needs to be dynamic in order to adapt to each PC configurations
    pthread_t* threads = (pthread_t*)calloc(nbOfThreads, sizeof(pthread_t));
    pthread_mutex_t inputMutex;
    pthread_mutex_t outputMutex;

    // Next, in order to synchronise each thread and prevent them from corrupting the result
    // We initialise two mutexes, one for reading the input and the other for writing
    // To the output
    pthread_mutex_init(&inputMutex, NULL);
    pthread_mutex_init(&outputMutex, NULL);

    struct SolveThreadArgs args = {
        inBuffer,
        outBuffer,
        table,    
        &inputMutex,
        &outputMutex,
        minimalOutput,
    };

    // We start each thread and store its handle in the thread array
    for (unsigned int threadID = 0; threadID < nbOfThreads; ++threadID) {
        pthread_create(&threads[threadID], NULL, solveThread , &args);
    }

    // And wait for each thread to exit
    for (unsigned int threadID = 0; threadID < nbOfThreads; ++threadID) {
        pthread_join(threads[threadID], NULL);
    }

    // We destroy each mutex
    pthread_mutex_destroy(&inputMutex);
    pthread_mutex_destroy(&outputMutex);

    // And finally free the thread array
    free(threads);

    // And close the file buffers
    closeOutFileBuffer(outBuffer);
    closeInFileBuffer(inBuffer);

    return 0;
}