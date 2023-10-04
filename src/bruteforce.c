#include "bruteforce.h"
#include "hash.h"

#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 256

const char* wordlistBF(const char* wordlistPath, const unsigned char targetDigest[32]) {
    FILE* wordlist = fopen(wordlistPath, "r");

    char buffer[BUFF_SIZE];
    unsigned char digest[32] = {0};

    char* password = NULL;
    while (!password && fgets(buffer, BUFF_SIZE, wordlist)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newlines;
        hash(digest, buffer);

        password = buffer;
        for (unsigned int index = 0; index < 32; ++index) {
            if (digest[index] != targetDigest[index]) {
                password = NULL;
                break;
            }
        }
    }

    fclose(wordlist);
    return password;
}