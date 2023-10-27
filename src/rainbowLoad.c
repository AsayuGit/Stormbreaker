#include "rainbow.h"

#include <string.h>

#include "outFileBuffer.h"
#include "inFileBuffer.h"

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