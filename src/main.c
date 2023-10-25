#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "rainbow.h"

#define ALGO_BUF_LEN 30

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

    char* inputPath = NULL;
    char* outputPath = NULL;
    char* tablePath = NULL;

    FILE* input = stdin;
    FILE* output = stdout;
    
    char algo[ALGO_BUF_LEN] = "SHA256";

    unsigned int nbOfThreads = 0;
    int status = 0;

    static struct option long_options[] = {
        {"help",  no_argument, 0, 'h'},
        {"Generate",  no_argument, 0, 'G'},
        {"Lookup",  no_argument, 0, 'L'},
        {"output",  required_argument, 0, 'o'},
        {"input",  required_argument, 0, 'i'},
        {"table",  required_argument, 0, 't'},
        {"algorithm",  required_argument, 0, 'a'},
        {0, 0, 0, 0}
    };

    char opt;
    int longopt_index;
    while ((opt = getopt_long(argc, argv, "hGLo:i:t:a:", long_options, &longopt_index)) != -1) {
        switch (opt) {
            case 'h':
                displayHelp();
                return 0;
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
                    return -1;
                } else strcpy(algo, optarg);
                break;
            case '?':
                return -1;
            default:
                break;
        }
    }

    if (inputPath) input = fopen(inputPath, "r");
    if (outputPath) output = fopen(outputPath, "w");

    // Mode G
    if (gMode) status = createRainbow(input, output, algo, nbOfThreads);

    // Mode L
    if (lMode) {
        FILE* tableFile = fopen(tablePath, "r");
        HashTable* rainbowTable = loadRainbow(tableFile);
        if (!rainbowTable) {
            fprintf(stderr, "ERROR: Unable to load table from input !\n");
            return -1;
        }

        status = solveRainbow(rainbowTable, input, output, nbOfThreads);
        freeHashTable(rainbowTable);

        fclose(tableFile);
    }

    if (inputPath) fclose(input);
    if (outputPath) fclose(output);

    return status;
}