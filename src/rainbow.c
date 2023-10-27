#include "rainbow.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <openssl/evp.h>

#include "stringTools.h"

#include "outFileBuffer.h"
#include "inFileBuffer.h"

#define BUFF_LEN 1024
#define STEP_RATE 2

#define IO_BUFFER_SIZE 0x10000000 // 256 MB

// This structs define the args passed to each thread
struct rainbowArgs {
    const EVP_MD* algorithm;      // Which hashing algorithm to use
    InFileBuffer* input;          // Input where to get the password from
    OutFileBuffer* output;        // Output where to write the dictionary to
    pthread_mutex_t* inputMutex;  // Read Mutex
    pthread_mutex_t* outputMutex; // Write Mutex
    size_t nbOfHashWritten;       // Total number of hash writen to the dictionary
    size_t step;                  // At which step to print out a new Info Log
    bool minimalOutput;           // Wether to restrict the output or not
};

struct SolveThreadArgs {
    InFileBuffer* input;          // Input where to read the hashes from
    OutFileBuffer* output;        // Output where to put the matches to
    HashTable* table;             // The loaded rainbow table
    pthread_mutex_t* inputMutex;  // Read Mutex
    pthread_mutex_t* outputMutex; // Write Mutex
    bool minimalOutput;           // Wether to restrict the output or not
};

// Wraps a mutex lock around (readLineInFileBuffer)
static int safeReadLineInFileBuffer(InFileBuffer* fileBuffer, char* buffer, size_t buffLen, pthread_mutex_t* mutex) {
    int status;
    
    pthread_mutex_lock(mutex); // Locks and unlock the mutex before attempting to access the shared ressource
    status = readLineInFileBuffer(fileBuffer, buffer, buffLen);
    pthread_mutex_unlock(mutex);

    return status;
}

// This is the function executed by each threads of createRainbow()
void* rainbowThread(void* args) {
    struct rainbowArgs* threadArgs = args;

    char password[BUFF_LEN];
    char base64Hash[BUFF_LEN];
    unsigned char digest[EVP_MAX_MD_SIZE];

    // We first init a new OpenSSL EVP Context for the hashing algorythm
    EVP_MD_CTX* shaContext = EVP_MD_CTX_new();

    // Then we Fetch a password from the input
    while (safeReadLineInFileBuffer(threadArgs->input, password, BUFF_LEN, threadArgs->inputMutex) != EOF) {

        // We compute its digest
        EVP_DigestInit(shaContext, threadArgs->algorithm);
        EVP_DigestUpdate(shaContext, password, strlen(password) * sizeof(char));
        unsigned int digestSize;
        EVP_DigestFinal(shaContext, digest, &digestSize);

        // Then its base64 representation
        for (unsigned int index = 0; index < digestSize; ++index) {
            sprintf(base64Hash + (index << 1), "%02x", digest[index]);
        }

        // Finally we write back the couple hash:password to the output
        pthread_mutex_lock(threadArgs->outputMutex);

        if (!threadArgs->minimalOutput) {
            writeOutFileBuffer(threadArgs->output, base64Hash, strlen(base64Hash));
            writeOutFileBuffer(threadArgs->output, ":", 1);
        }
        
        writeOutFileBuffer(threadArgs->output, password, strlen(password));
        writeOutFileBuffer(threadArgs->output, "\n", 1);
        
        // We periodically keep track of the number of hash written
        threadArgs->nbOfHashWritten++;
        if (threadArgs->nbOfHashWritten >= threadArgs->step) {
            printf("INFO: %ld hashes written...\n", threadArgs->nbOfHashWritten);
            threadArgs->step *= STEP_RATE;
        }
        
        pthread_mutex_unlock(threadArgs->outputMutex);
    }

    // And free the EVP Context
    EVP_MD_CTX_free(shaContext);

    return NULL;
}

// Create dictionary file from the input and saves it to the output
int createRainbow(FILE* input, FILE* output, const char* algorithm, unsigned int nbOfThreads, bool minimalOutput) {
    if (!input || !output) return -1;
    if (nbOfThreads == 0) nbOfThreads = get_nprocs();

    // First, we initialize an array of pthread_t* to store the handle of each thread
    // This needs to be dynamic in order to adapt to each PC configurations
    pthread_t* threads = (pthread_t*)calloc(nbOfThreads, sizeof(pthread_t));
    pthread_mutex_t inputMutex;
    pthread_mutex_t outputMutex;

    // Then, in order to synchronise each thread and prevent them from corrupting the result
    // We initialise two mutexes, one for reading the input and the other for writing
    // To the output
    pthread_mutex_init(&inputMutex, NULL);
    pthread_mutex_init(&outputMutex, NULL);

    // We fetch the required digest type
    const EVP_MD* algo = EVP_get_digestbyname(algorithm);
    if (!algo) {
        fprintf(stderr, "FATAL: Digest Invalid !\n");
        return -1;
    }

    // Allocate I/O File buffers
    OutFileBuffer* fileBuffer = openOutFileBuffer(output, IO_BUFFER_SIZE);
    InFileBuffer* inFileBuffer = openInFileBuffer(input, IO_BUFFER_SIZE);

    // And prepare the shared threads arguments
    struct rainbowArgs args = {
        algo,
        inFileBuffer,
        fileBuffer,
        &inputMutex,
        &outputMutex,
        0,
        10,
        minimalOutput
    };

    // We start each thread and store its handle in the thread array
    for (unsigned int threadID = 0; threadID < nbOfThreads; ++threadID) {
        pthread_create(&threads[threadID], NULL, rainbowThread, &args);
    }

    // And wait for each thread to exit
    for (unsigned int threadID = 0; threadID < nbOfThreads; ++threadID) {
        pthread_join(threads[threadID], NULL);
    }

    // At the end print the total number of hashes written
    printf("INFO: %ld hashes written.\n", args.nbOfHashWritten);

    // We destroy each mutex
    pthread_mutex_destroy(&inputMutex);
    pthread_mutex_destroy(&outputMutex);

    // And finally free the thread array
    free(threads);

    // And close the file buffers
    closeInFileBuffer(inFileBuffer);
    closeOutFileBuffer(fileBuffer);

    return 0;
}

// Load a rainbow table from a dictionary file
HashTable* loadRainbow(FILE* input) {
    if (!input) return NULL; // There is nothing to load if there is no input

    // First, we allocate a input file buffer
    InFileBuffer* fileBuffer = openInFileBuffer(input, IO_BUFFER_SIZE);

    // Then figure the total number of lines in the file
    size_t lineCount = getLineCountInFileBuffer(fileBuffer);
    printf("DEBUG: linecount %ld\n", lineCount);
    
    // Next, we attempt to create the hash table big enough to store all hashes
    HashTable* table = createHashTable(lineCount);
    if (!table) {
        // If we fail we error out
        fprintf(stderr, "FATAL: Unable to create hashtable !\n");
        return NULL; // and return
    }
    
    size_t nbOfHashLoaded = 0;
    size_t step = 10;
    char buffer[BUFF_LEN];

    // Fetch each combo hash:password from the input
    while (readLineInFileBuffer(fileBuffer, buffer, BUFF_LEN) != EOF) {
        // Separate the line into its two components
        char* hash = strtok(buffer, ":");
        char* password = strtok(NULL, "\n");

        insertHashTable(table, hash, password); // Then insert the data into the hash table
        ++nbOfHashLoaded; // And keep tract of the number of loaded hashes
        if (nbOfHashLoaded >= step) {
            printf("INFO: Loading %ld hashes...\n", nbOfHashLoaded);
            step *= STEP_RATE;
        }
    }

    // At the end of the process we print out the total number of hash loaded in the hash table
    printf("INFO: %ld hashs loaded.\n", nbOfHashLoaded);
    closeInFileBuffer(fileBuffer);

    return table; // Finaly return the fully loaded rainbow table
}

// This is the function executed by each threads of solveRainbow()
void* solveThread(void* args) {
    struct SolveThreadArgs* threadArgs = args;

    char key[BUFF_LEN];
    char* data;
    // For each line of the input
    while (safeReadLineInFileBuffer(threadArgs->input, key, BUFF_LEN, threadArgs->inputMutex) != EOF) {
        // Ensure the hash is in lowercase
        strToLower(key);
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