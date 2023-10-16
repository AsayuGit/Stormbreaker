#include "rainbow.h"

#include <stdio.h>
#include <string.h>

#include "hash.h"

#define BUFF_LEN 256
#define NB_OF_STAGES 4

int fetchLine(FILE* file, char* buffer) {
    unsigned int buffIndex = 0;
    char c = fgetc(file);
    for (; (c != EOF) && (c != '\n'); c = fgetc(file)) {
        buffer[buffIndex++] = c;
    }
    buffer[buffIndex] = '\0';
    
    return (buffIndex) ? buffIndex : EOF;
}

void createRainbow(const char* wordlistInPath, const char* rainbowOutPath) {
    if (!wordlistInPath || !rainbowOutPath) return;

    FILE* input = fopen(wordlistInPath, "r"); // Open wordlist (r)
    FILE* output = fopen(rainbowOutPath, "w"); // Open rainbow (w)
    if (!input | !output) return;

    char password[BUFF_LEN];
    char reducedPassword[65];
    unsigned char digest[32];

    // Fetch a password
    while (fetchLine(input, password) != EOF) {
        // Compute pass couple

        strcpy(reducedPassword, password);
        for (unsigned int i = 0; i < NB_OF_STAGES; ++i) {
            // 1. hash password
            hash(digest, reducedPassword);

            // 2. reduce hash
            digestToCString(digest, reducedPassword);
            
            // 3. repeat as needed
        }

        // write back
        fprintf(output, "%s:%s\n", reducedPassword, password);
    }

    // Close files
    fclose(output);
    fclose(input);
}