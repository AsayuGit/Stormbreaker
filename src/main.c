#include <stdlib.h>
#include <stdio.h>

#include "rainbow.h"
#include "bruteforce.h"
#include "hash.h"

int main(int argc, char** argv) {
    //createRainbow("rockyou.txt", "rainbow.txt");
    printf("Loading Table...\n");
    HashTable* rainbowTable = loadRainbow("rainbow.txt");
    printf("Table Loaded !\n");
    
    printf("\nRainbow : %p\n", rainbowTable);

    char* data = getHashTable(rainbowTable, "B54A1AF8B666F61C2DD5AE8F8A543133409FD28C3B78064C5DB993BF2C8E77BC");
    printf("Data: %s\n", data);


    freeHashTable(rainbowTable);
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