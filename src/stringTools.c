#include "stringTools.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Char by char to get arround "corrupted" strings
// fgets() would exit mid line
int fetchLine(FILE* file, char* buffer, size_t buffLen) {
    if (!buffer || !buffLen) return EOF;
    --buffLen;

    unsigned int buffIndex = 0;
    char c;
    for (c = getc(file); c != EOF; c = getc(file)) { // for each char of the file
        if (c == '\n') break; // Lines are terminated by '\n'
        buffer[buffIndex] = c; // Copy the char to the line buffer
        ++buffIndex;           // Then increment the buffer index
        if (buffIndex > buffLen) break; // Lines can't be bigger than the buffer
    }

    buffer[buffIndex] = '\0'; // Terminate the fetched line by '\0'
    return c; // And retrun the last read char
}

// Returns the number of lines of a file
size_t getLineCount(FILE* file) {
    if (!file) return 0; // No file, no lines

    // Save the file position before compute
    fpos_t pos;
    fgetpos(file, &pos);

    fseek(file, 0, SEEK_SET); // Go back to begining of the file

    size_t linecount = 0;
    for (char c = getc(file); c != EOF; c = getc(file)) {
        if (c == '\n') ++linecount; // Increment the line count each time a '\n' is found
    }

    fsetpos(file, &pos); // restore the original file postion after compute

    return linecount;
}

// Convert a string from [UPPERCASE] to [lowercase]
void strToLower(char* cstring) {
    for (; *cstring != '\0'; ++cstring) {
        *cstring = tolower(*cstring);
    }
}