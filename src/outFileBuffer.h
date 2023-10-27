#ifndef _OUTFILEBUFFER_H
#define _OUTFILEBUFFER_H

#include <stdio.h>

typedef struct OutFileBuffer OutFileBuffer;

OutFileBuffer* openOutFileBuffer(FILE* file, size_t size);
void writeOutFileBuffer(OutFileBuffer* fileBuffer, const char* data, size_t size);
void flushOutFileBuffer(OutFileBuffer* fileBuffer);
void closeOutFileBuffer(OutFileBuffer* fileBuffer);

#endif