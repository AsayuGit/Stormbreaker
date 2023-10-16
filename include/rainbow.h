#ifndef _RAINBOW_H
#define _RAINBOW_H

#include "hashTable.h"

void createRainbow(const char* wordlistInPath, const char* rainbowOutPath);
void createRainbowEx(const char* wordlistInPath, const char* rainbowOutPath, unsigned int nbOfThreads);

HashTable* loadRainbow(const char* rainbowPath);

#endif