#include <stdio.h>

#include "bruteforce.h"
#include "hash.h"

int main(int argc, char** argv) {
    unsigned char targetDigest[32];

    hash(targetDigest, "hello36");
    const char* pass = wordlistBF("rockyou.txt", targetDigest);
    
    if (pass) {
        printf("Password is : %s\n", pass);
        return 0;
    }

    printf("Sorry but password not found :c\n");
    return 0;
}