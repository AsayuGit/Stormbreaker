#ifndef _RAINBOW_H
#define _RAINBOW_H

#include "hashTable.h"

#include <stdio.h>
#include <stdbool.h>

int createRainbow(FILE* input, FILE* output, const char* algorithm, unsigned int nbOfThreads, bool minimalOutput);
HashTable* loadRainbow(FILE* input);
int solveRainbow(HashTable* table, FILE* input, FILE* output, unsigned int nbOfThreads, bool minimalOutput);

#endif