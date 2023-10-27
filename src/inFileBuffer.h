#ifndef _INFILEBUFFER_H
#define _INFILEBUFFER_H

#include <stdio.h>

typedef struct InFileBuffer InFileBuffer;

InFileBuffer* openInFileBuffer(FILE* file, size_t size);
size_t readInFileBuffer(InFileBuffer* fileBuffer, char* data, size_t size);
void seekStartInFileBuffer(InFileBuffer* fileBuffer);
size_t getLineCountInFileBuffer(InFileBuffer* fileBuffer);
int readLineInFileBuffer(InFileBuffer* fileBuffer, char* buffer, size_t buffLen);
void closeInFileBuffer(InFileBuffer* fileBuffer);

#endif