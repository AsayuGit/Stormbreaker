#include "inFileBuffer.h"

#include <stdlib.h>
#include <string.h>

// Define the InFileBuffer struct
// To handle a read buffer from a file stream
struct InFileBuffer {
    FILE* file;          // The bufferized stream
    char* buffer;        // The buffer itself
    size_t size;         // The size of the allocated buffer
    size_t bufferIndex;  // The current position within the buffer
    size_t bufferedSize; // The size of the currently buffered data
    size_t pageIndex;    // The index of the current page
};

// Fetches a new page from disk to the buffer
static void fetchPageInFileBuffer(InFileBuffer* fileBuffer) {
    // Try to read the full size of the allocated buffer
    fileBuffer->bufferedSize = fread(fileBuffer->buffer, sizeof(char), fileBuffer->size, fileBuffer->file);
    fileBuffer->bufferIndex = 0; // Then set the bufferIndex to the start of the buffer
    ++fileBuffer->pageIndex;
}

// Allocate a read buffer on a file stream
InFileBuffer* openInFileBuffer(FILE* file, size_t size) {
    // We can't open a buffer without a file stream
    // Likewise a buffer of size 0 makes no sense
    if (!file || !size) return NULL;
    InFileBuffer* newBufferedFile;

    // We first try to allocate the InFileBuffer struct itself
    if (!(newBufferedFile = (InFileBuffer*)malloc(sizeof(InFileBuffer)))) {
        return NULL; // And exit in case of failure
    }

    // Then we try to allocate the buffer with the specified size
    if (!(newBufferedFile->buffer = (char*)malloc(size * sizeof(char)))) {
        // In case of failure we first free the previously allocated InFileBuffer struct
        free(newBufferedFile);
        return NULL; // Then exit
    }

    // If we were successful we set the required data
    newBufferedFile->file = file;
    newBufferedFile->size = size;
    newBufferedFile->pageIndex = 0;

    // And imediatly fetch a page from disk
    fetchPageInFileBuffer(newBufferedFile);

    return newBufferedFile;
}

// Wraps a mutex lock around (readLineInFileBuffer)
int safeReadLineInFileBuffer(InFileBuffer* fileBuffer, char* buffer, size_t buffLen, pthread_mutex_t* mutex) {
    int status;
    
    pthread_mutex_lock(mutex); // Locks and unlock the mutex before attempting to access the shared ressource
    status = readLineInFileBuffer(fileBuffer, buffer, buffLen);
    pthread_mutex_unlock(mutex);

    return status;
}

// Read some data from the buffer and fetches more if necessary
size_t readInFileBuffer(InFileBuffer* fileBuffer, char* data, size_t size) {
    // If the data to read is bigger than the remaining data in the buffer
    if (fileBuffer->bufferIndex + size > fileBuffer->bufferedSize) {
        // Figure out the size of the segment to read
        size_t segmentSize = fileBuffer->bufferedSize - fileBuffer->bufferIndex;

        // Read that segment from the buffer
        memcpy(data, fileBuffer->buffer + fileBuffer->bufferIndex, segmentSize);

        // Then as long as bufferedSize == size there may be more data in the file
        if (fileBuffer->bufferedSize < fileBuffer->size) {
            // if thre is not set the bufferedSize to 0
            fileBuffer->bufferIndex = 0;
            fileBuffer->bufferedSize = 0;

            // And return the previously read data size
            return segmentSize;
        }

        // If there is then fetch a new page from the file
        fetchPageInFileBuffer(fileBuffer);

        // And read the rest of the data returning its size plus the previously read data size
        return segmentSize + readInFileBuffer(fileBuffer, data + segmentSize, size - segmentSize);
    }

    // if not then simply read the data from the buffer
    memcpy(data, fileBuffer->buffer + fileBuffer->bufferIndex, size);
    fileBuffer->bufferIndex += size;

    // And return the read data size
    return size;
}

// Set the buffer back to the start of the file
void seekStartInFileBuffer(InFileBuffer* fileBuffer) {
    // If the buffer is not on the first page
    if (fileBuffer->pageIndex > 1) {
        // Seek back to begining of the file
        fseek(fileBuffer->file, 0, SEEK_SET);
        fileBuffer->pageIndex = 0;

        // And fetch the first page
        fetchPageInFileBuffer(fileBuffer);
    } else {
        // if it is then simply set the buffer index to the start of the buffer
        fileBuffer->bufferIndex = 0;
    }
}

// Get the total number of lines in the buffered file
size_t getLineCountInFileBuffer(InFileBuffer* fileBuffer) {
    if (!fileBuffer) return 0; // No file, no lines

    size_t linecount = 0;
    
    // First go back to begining of the file
    seekStartInFileBuffer(fileBuffer);

    // Loop across pages
    while (1) {
        // For the current buffered data
        for (; fileBuffer->bufferIndex < fileBuffer->bufferedSize; ++fileBuffer->bufferIndex) {
            // If we find a '\n' then increment the line count
            if (fileBuffer->buffer[fileBuffer->bufferIndex] == '\n') ++linecount;
        } // At the end of the current buffer
        
        // If there is no more data to fetch
        if (fileBuffer->bufferedSize < fileBuffer->size) break; // Break out the while loop
        // If there is then fetch another page and loop around
        fetchPageInFileBuffer(fileBuffer);
    } // Once all the pages have been checked

    // Go back to begining of the file
    seekStartInFileBuffer(fileBuffer);

    // And return the total line count
    return linecount;
}

// Reads a line from the buffer and fetch a new page if necessary
int readLineInFileBuffer(InFileBuffer* fileBuffer, char* buffer, size_t buffLen) {
    // You can't read a line without the proper arguments
    if (!fileBuffer || !buffer || !buffLen) return EOF;
    // Save space for '\0'
    --buffLen;

    char c = EOF;
    // For each page of the file
    while (1) {
        // For the current buffered data
        for (; fileBuffer->bufferIndex < fileBuffer->bufferedSize; ++buffer) {
            // Fetch a char from the buffer
            c = fileBuffer->buffer[fileBuffer->bufferIndex];
            ++fileBuffer->bufferIndex;

            // If it is the end of the line
            if (c == '\n') {
                // End the line buffer
                (*buffer) = '\0';
                return c; // And exit returning the last read char
            }

            // Else copy the current char to the line buffer
            (*buffer) = c;
        }
        // If there is no more data to fetch
        if (fileBuffer->bufferedSize < fileBuffer->size) break; // Break out the while loop
        // If there is then fetch another page and loop around
        fetchPageInFileBuffer(fileBuffer);
    } // After checking all pages

    // End the line buffer
    (*buffer) = '\0';
    return c; // And return the last read char
}

// close a previously allocated read buffer
void closeInFileBuffer(InFileBuffer* fileBuffer) {
    // Can't close a non-existant buffer
    if (!fileBuffer) return;
    // First free the allocated buffer
    free(fileBuffer->buffer);
    // Then free the InFileBuffer struct
    free(fileBuffer);
}