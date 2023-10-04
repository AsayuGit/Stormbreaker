#ifndef _HASH_H
#define _HASH_H

void hashCString(char outbuffer[65], const char* inbuffer);
void hash(unsigned char digest[32], const char* inbuffer);

#endif