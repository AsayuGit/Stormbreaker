#ifndef _RAINBOW_H
#define _RAINBOW_H

#include "hashTable.h"

void createRainbow(const char* wordlistInPath, const char* rainbowOutPath, const char* algorithm);
void createRainbowEx(const char* wordlistInPath, const char* rainbowOutPath, const char* algorithm, unsigned int nbOfThreads);

HashTable* loadRainbow(const char* rainbowPath);

#endif