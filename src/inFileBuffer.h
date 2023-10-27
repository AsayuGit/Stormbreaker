#ifndef _INFILEBUFFER_H
#define _INFILEBUFFER_H

#include <stdio.h>
#include <pthread.h>

typedef struct InFileBuffer InFileBuffer;

InFileBuffer* openInFileBuffer(FILE* file, size_t size);
int safeReadLineInFileBuffer(InFileBuffer* fileBuffer, char* buffer, size_t buffLen, pthread_mutex_t* mutex);
size_t readInFileBuffer(InFileBuffer* fileBuffer, char* data, size_t size);
void seekStartInFileBuffer(InFileBuffer* fileBuffer);
size_t getLineCountInFileBuffer(InFileBuffer* fileBuffer);
int readLineInFileBuffer(InFileBuffer* fileBuffer, char* buffer, size_t buffLen);
void closeInFileBuffer(InFileBuffer* fileBuffer);

#endif