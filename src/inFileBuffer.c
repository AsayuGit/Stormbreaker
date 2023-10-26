#include "inFileBuffer.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <stdio.h>

struct InFileBuffer {
    FILE* file;
    char* buffer;
    size_t size;
    size_t bufferIndex;
    size_t bufferedSize;
};

static void fetchPageInFileBuffer(InFileBuffer* fileBuffer) {
    fileBuffer->bufferedSize = fread(fileBuffer->buffer, sizeof(char), fileBuffer->size, fileBuffer->file);
    fileBuffer->bufferIndex = 0;
}

InFileBuffer* openInFileBuffer(FILE* file, size_t size) {
    if (!file) return NULL;
    InFileBuffer* newBufferedFile;

    if (!(newBufferedFile = (InFileBuffer*)malloc(sizeof(InFileBuffer)))) {
        return NULL;
    }

    if (!(newBufferedFile->buffer = (char*)malloc(size * sizeof(char)))) {
        free(newBufferedFile);
        return NULL;
    }

    newBufferedFile->file = file;
    newBufferedFile->size = size;

    fetchPageInFileBuffer(newBufferedFile);

    return newBufferedFile;
}

size_t readInFileBuffer(InFileBuffer* fileBuffer, char* data, size_t size) {
    if (fileBuffer->bufferIndex + size > fileBuffer->bufferedSize) {
        size_t segmentSize = fileBuffer->bufferedSize - fileBuffer->bufferIndex;
        memcpy(data, fileBuffer->buffer + fileBuffer->bufferIndex, segmentSize);

        if (fileBuffer->bufferedSize < fileBuffer->size) {
            fileBuffer->bufferIndex = 0;
            fileBuffer->bufferedSize = 0;
            return segmentSize;
        }

        fetchPageInFileBuffer(fileBuffer);
        return segmentSize + readInFileBuffer(fileBuffer, data + segmentSize, size - segmentSize);
    }

    memcpy(data, fileBuffer->buffer + fileBuffer->bufferIndex, size);
    fileBuffer->bufferIndex += size;

    return size;
}

size_t getLineCountInFileBuffer(InFileBuffer* fileBuffer) {
    printf("Asserting file size\n");
    if (!fileBuffer) return 0; // No file, no lines

    size_t linecount = 0;
    fseek(fileBuffer->file, 0, SEEK_SET); // Go back to begining of the file
    do {
        fetchPageInFileBuffer(fileBuffer);
        for (; fileBuffer->bufferIndex < fileBuffer->bufferedSize; ++fileBuffer->bufferIndex) {
            if (fileBuffer->buffer[fileBuffer->bufferIndex] == '\n') ++linecount;
        }
    } while (fileBuffer->bufferedSize == fileBuffer->size);

    fseek(fileBuffer->file, 0, SEEK_SET); // Go back to begining of the file
    fetchPageInFileBuffer(fileBuffer);

    printf("Asserting file size DONE\n");
    return linecount;
}

int readLineInFileBuffer(InFileBuffer* fileBuffer, char* buffer, size_t buffLen) {
    if (!fileBuffer || !buffer || !buffLen) return EOF;
    --buffLen;

    char c = EOF;
    for (; fileBuffer->bufferedSize; fetchPageInFileBuffer(fileBuffer)) {
        for (; fileBuffer->bufferIndex < fileBuffer->bufferedSize; ++buffer) {
            c = fileBuffer->buffer[fileBuffer->bufferIndex];
            ++fileBuffer->bufferIndex;
            if (c == '\n') {
                (*buffer) = '\0';
                return c;
            }
            (*buffer) = c;
        }
        
    }

    (*buffer) = '\0';
    return c;
}

void closeInFileBuffer(InFileBuffer* fileBuffer) {
    if (!fileBuffer) return;
    free(fileBuffer->buffer);
    free(fileBuffer);
}