#include "rainbow.h"

#include <openssl/evp.h>
#include <string.h>
#include <sys/sysinfo.h>

#include "outFileBuffer.h"
#include "inFileBuffer.h"

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

        writeOutFileBuffer(threadArgs->output, base64Hash, strlen(base64Hash));

        if (!threadArgs->minimalOutput) {
            writeOutFileBuffer(threadArgs->output, ":", 1);
            writeOutFileBuffer(threadArgs->output, password, strlen(password));
        }
        
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