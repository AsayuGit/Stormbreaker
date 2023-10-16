#ifndef _HASH_H
#define _HASH_H

#include <stddef.h>

void hashCString(char outbuffer[65], const char* inbuffer);
void hash(unsigned char digest[32], const char* inbuffer);
void digestToCString(unsigned char digest[32], char cstring[65]);

unsigned int hashIndex(const char* data, size_t size);

#endif