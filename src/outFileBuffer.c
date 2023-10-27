#include "outFileBuffer.h"

#include <stdlib.h>
#include <string.h>

// Define the OutFileBuffer struct
// To handle a write buffer to a file stream
struct OutFileBuffer {
    FILE* file;         // The bufferized stream
    char* buffer;       // The buffer itself
    size_t size;        // The size of the allocated buffer
    size_t bufferIndex; // The current position within the buffer
};

// Allocate a write buffer on a file stream
OutFileBuffer* openOutFileBuffer(FILE* file, size_t size) {
    // We can't open a buffer without a file stream
    // Likewise a buffer of size 0 makes no sense
    if (!file || !size) return NULL;
    OutFileBuffer* newWriteBuffer;

    // We first try to allocate the OutFileBuffer struct itself
    if (!(newWriteBuffer = (OutFileBuffer*)malloc(sizeof(OutFileBuffer)))) {
        return NULL; // And imediatly exit in case of failure
    }

    // Then we try to allocate the buffer with the specified size
    if (!(newWriteBuffer->buffer = (char*)malloc(size * sizeof(char)))) {
        // In case of failure we free the previously allocated OutFileBuffer struct
        free(newWriteBuffer);
        return NULL; // Then exit
    }

    // If we successfuly allocate the buffer we set the required data
    newWriteBuffer->file = file;
    newWriteBuffer->size = size;
    newWriteBuffer->bufferIndex = 0; // And place ourselves at the start of the buffer

    // Then finally return the newly allocated write buffer
    return newWriteBuffer;
}

// Write some data to the buffer and flush if necessary
void writeOutFileBuffer(OutFileBuffer* fileBuffer, const char* data, size_t size) {
    // If the data to write is bigger than the remaining space in the buffer
    if (fileBuffer->bufferIndex + size > fileBuffer->size) {
        // Figure out the size of the segment to write
        size_t segmentSize = fileBuffer->size - fileBuffer->bufferIndex;

        // Write that segment to the buffer
        memcpy(fileBuffer->buffer + fileBuffer->bufferIndex, data, segmentSize);
        fileBuffer->bufferIndex = fileBuffer->size;    
        
        // And flush the buffer to disk
        flushOutFileBuffer(fileBuffer);

        // Then restart the process with the rest of the data
        writeOutFileBuffer(fileBuffer, data + segmentSize, size - segmentSize);
        return; // And exit
    }

    // If not then simply write it to the buffer
    memcpy(fileBuffer->buffer + fileBuffer->bufferIndex, data, size);
    fileBuffer->bufferIndex += size;
}

// Flush the buffer to disk
void flushOutFileBuffer(OutFileBuffer* fileBuffer) {
    // Can't flush a non-existant buffer
    if (!fileBuffer) return;
    // Write the written part of the buffer directly to disk
    fwrite(fileBuffer->buffer, sizeof(char), fileBuffer->bufferIndex, fileBuffer->file);
    // Then set the buffer index back to the start of the buffer
    fileBuffer->bufferIndex = 0;
}

// Flush and close a previously allocated write buffer
void closeOutFileBuffer(OutFileBuffer* fileBuffer) {
    // Can't close an invalid buffer
    if (!fileBuffer) return;
    // We first flush any remaining data from the buffer to the disk
    flushOutFileBuffer(fileBuffer);
    // Then we free the buffer itself
    free(fileBuffer->buffer);
    // And finally the OutFileBuffer struct
    free(fileBuffer);
}