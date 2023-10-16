#include <stdlib.h>
#include <stdio.h>

#include "rainbow.h"
#include "bruteforce.h"
#include "hash.h"

int main(int argc, char** argv) {
    createRainbow("tiny.txt", "rainbow.txt");
    return 0;

    unsigned char targetDigest[32];

    hash(targetDigest, " fuckyooh21");
    char* pass = wordlistBF("rockyou.txt", targetDigest);
    
    if (pass) {
        printf("Password is : >%s<\n", pass);
        free(pass);
        return 0;
    }

    printf("Sorry but password not found :c\n");
    return 0;
}