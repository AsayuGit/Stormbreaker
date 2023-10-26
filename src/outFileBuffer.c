#include "outFileBuffer.h"

#include <stdlib.h>
#include <string.h>

struct OutFileBuffer {
    FILE* file;
    char* buffer;
    size_t size;
    size_t bufferIndex;
};

OutFileBuffer* openOutFileBuffer(FILE* file, size_t size) {
    if (!file) return NULL;
    OutFileBuffer* newBufferedFile;

    if (!(newBufferedFile = (OutFileBuffer*)malloc(sizeof(OutFileBuffer)))) {
        return NULL;
    }

    if (!(newBufferedFile->buffer = (char*)malloc(size * sizeof(char)))) {
        free(newBufferedFile);
        return NULL;
    }

    newBufferedFile->file = file;
    newBufferedFile->size = size;
    newBufferedFile->bufferIndex = 0;

    return newBufferedFile;
}

void writeOutFileBuffer(OutFileBuffer* fileBuffer, const char* data, size_t size) {
    if (fileBuffer->bufferIndex + size > fileBuffer->size) {
        size_t segmentSize = fileBuffer->size - fileBuffer->bufferIndex;
        memcpy(fileBuffer->buffer + fileBuffer->bufferIndex, data, segmentSize);
        fileBuffer->bufferIndex = fileBuffer->size;    
        flushOutFileBuffer(fileBuffer);
        data += segmentSize;
        size -= segmentSize;
    }

    memcpy(fileBuffer->buffer + fileBuffer->bufferIndex, data, size);
    fileBuffer->bufferIndex += size;
}

void flushOutFileBuffer(OutFileBuffer* fileBuffer) {
    fwrite(fileBuffer->buffer, sizeof(char), fileBuffer->bufferIndex, fileBuffer->file);
    fileBuffer->bufferIndex = 0;
}

void closeOutFileBuffer(OutFileBuffer* fileBuffer) {
    if (!fileBuffer) return;
    flushOutFileBuffer(fileBuffer);
    free(fileBuffer->buffer);
    free(fileBuffer);
}