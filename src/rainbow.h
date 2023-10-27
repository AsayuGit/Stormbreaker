#ifndef _RAINBOW_H
#define _RAINBOW_H

#include "hashTable.h"

#include <stdio.h>
#include <stdbool.h>

#define BUFF_LEN 1024
#define STEP_RATE 2

#define IO_BUFFER_SIZE 0x10000000 // 256 MB

int createRainbow(FILE* input, FILE* output, const char* algorithm, unsigned int nbOfThreads, bool minimalOutput);
HashTable* loadRainbow(FILE* input);
int solveRainbow(HashTable* table, FILE* input, FILE* output, unsigned int nbOfThreads, bool minimalOutput);

#endif