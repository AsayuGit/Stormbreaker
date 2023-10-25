#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "rainbow.h"

#define ALGO_BUF_LEN 30

// display the help menu whenever the -h option is used
void displayHelp() {
    printf(
        "bforceSha: [mode] [options]\n"
        "EG: ./bfroceSha -G -a SHA256 -o rainbow.table\n\n"
        "\t-h/--help:      Display this help message\n"
        "\t-G/--Generate:  Generate a hash table\n"
        "\t-L/--Lookup:    Lookup an hash to find its originating password\n"
        "\t-o/--output:    Set the output file for the operation\n"
        "\t-i/--input:     Set the input file for the operation\n"
        "\t-t/--table:     Set the table file in L mode\n"
        "\t-a/--algorythm: Set the algorythm used for generation\n"
    );
}

int main(int argc, char** argv) {
    // Program Flags
    bool gMode = false;
    bool lMode = false;

    // Optional file paths
    char* inputPath = NULL;
    char* outputPath = NULL;
    char* tablePath = NULL;

    // Init streams to the standard console if no path is specified
    FILE* input = stdin;
    FILE* output = stdout;
    
    // Algorythm used by the hashing function
    char algo[ALGO_BUF_LEN] = "SHA256"; // Set the default Algorythm to SHA256

    // The number of threads to use for generation (0 = As many as available hardware threads)
    unsigned int nbOfThreads = 0;
    
    // Program return status
    int status = 0;

    // Available command line options
    static struct option long_options[] = {
        {"help",  no_argument, 0, 'h'},
        {"Generate",  no_argument, 0, 'G'},
        {"Lookup",  no_argument, 0, 'L'},
        {"output",  required_argument, 0, 'o'},
        {"input",  required_argument, 0, 'i'},
        {"table",  required_argument, 0, 't'},
        {"algorithm",  required_argument, 0, 'a'},
        {"jobs",  required_argument, 0, 'j'},
        {0, 0, 0, 0}
    };

    char opt;
    int longopt_index;
    // Parse each command line argument and set the appropriate flags
    while ((opt = getopt_long(argc, argv, "hGLo:i:t:a:j:", long_options, &longopt_index)) != -1) {
        switch (opt) {
            case 'h':
                displayHelp();
                status = 0;
                break;
            case 'G':
                gMode = true;
                break;
            case 'L':
                lMode = true;
                break;
            case 'o':
                outputPath = optarg;
                break;
            case 'i':
                inputPath = optarg;
                break;
            case 't':
                tablePath = optarg;
                break;
            case 'a':
                if (strlen(optarg) > ALGO_BUF_LEN) {
                    fprintf(stderr, "Algorythm Invalid\n");
                    status = -1;
                } else strcpy(algo, optarg);
                break;
            case 'j':
                int intarg = atoi(optarg); 
                if (intarg > 0) {
                    nbOfThreads = intarg;
                } else {
                    fprintf(stderr, "FATAL: Invalid number of threads\n");
                    status = -1;
                }
                break;
            case '?':
                status = -1;
                break;
            default:
                break;
        }
    }

    // Try to open the input file if specified
    if (!status && inputPath) {
        if (!(input = fopen(inputPath, "r"))) {
            fprintf(stderr, "FATAL: Cannot read file %s\n", inputPath);
            status = -1;
        }
    }

    // Try to open the output file if specified
    if (!status && outputPath) {
        if (!(output = fopen(outputPath, "w"))) {
            fprintf(stderr, "FATAL: Cannot write file %s\n", outputPath);
            status = -1;
        }
    }

    // Mode G
    if (!status && gMode) status = createRainbow(input, output, algo, nbOfThreads);

    // Mode L
    if (!status && lMode) {
        // Load the specified dict file
        printf("INFO: Loading dict file: %s\n", tablePath);
        FILE* tableFile = fopen(tablePath, "r");
        HashTable* rainbowTable = loadRainbow(tableFile);
        if (!rainbowTable) {
            fprintf(stderr, "FATAL: Cannot load dict file %s\n", tablePath);
            return -1;
        }
        printf("INFO: Ready !\n");

        // Try to solve each input hash
        status = solveRainbow(rainbowTable, input, output, nbOfThreads);
        
        // Free the previously loaded dict file
        freeHashTable(rainbowTable);
        fclose(tableFile);
    }

    // Close open files if any
    if (inputPath) fclose(input);
    if (outputPath) fclose(output);

    return status;
}