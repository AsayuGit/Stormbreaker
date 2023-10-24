#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "rainbow.h"
#include "bruteforce.h"
#include "hash.h"

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
        "\t-a/--algorythm: Set the algorythm used for generation\n"
    );
}

int main(int argc, char** argv) {
    // Program Flags
    bool gMode = false;
    bool lMode = false;

    char* output = "rainbow.table";
    char* input = "rainbow.table";
    char algo[ALGO_BUF_LEN] = "SHA256";

    static struct option long_options[] = {
        {"help",  no_argument, 0, 'h'},
        {"Generate",  no_argument, 0, 'G'},
        {"Lookup",  no_argument, 0, 'L'},
        {"output",  required_argument, 0, 'o'},
        {"input",  required_argument, 0, 'i'},
        {"algorithm",  required_argument, 0, 'a'},
        {0, 0, 0, 0}
    };

    char opt;
    int longopt_index;
    while ((opt = getopt_long(argc, argv, "hGLo:i:a:", long_options, &longopt_index)) != -1) {
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
                output = optarg;
                break;
            case 'i':
                input = optarg;
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


    // Mode G
    if (gMode) createRainbow("rockyou.txt", output, algo);

    // Mode L
    if (lMode) {
        printf("Loading Table...\n");
        HashTable* rainbowTable = loadRainbow(input);
        if (!rainbowTable) {
            fprintf(stderr, "ERROR: Unable to load %s !\n", input);
            return -1;
        }

        printf("Table Loaded !\n");
        
        char* data = getHashTable(rainbowTable, "350A21E0A8F81DA2BD8E25E418B96D527C0C352D73B43A5A1E9F1B8FEA9EB43C");
        printf("Data: %s\n", data);


        freeHashTable(rainbowTable);
    }

    return 0;
}