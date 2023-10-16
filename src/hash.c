#include "hash.h"

#include <string.h>
#include <openssl/evp.h>

void digestToCString(unsigned char digest[32], char cstring[65]) {
    for (unsigned int index = 0; index < 32; ++index) {
        sprintf(cstring + (index << 1), "%02X", digest[index]);
    }
}

void hash(unsigned char digest[32], const char* inbuffer) {
    const EVP_MD* digestType = EVP_get_digestbyname("SHA256");

    EVP_MD_CTX* shaContext = EVP_MD_CTX_new();
    EVP_DigestInit(shaContext, digestType);

    for (unsigned int index = 0; index < strlen(inbuffer); ++index) {
        EVP_DigestUpdate(shaContext, inbuffer + index, sizeof(char));
    }

    EVP_DigestFinal(shaContext, digest, NULL);
    EVP_MD_CTX_free(shaContext);
}

void hashCString(char outbuffer[65], const char* inbuffer) {
    unsigned char digest[32];
    
    hash(digest, inbuffer);
    digestToCString(digest, outbuffer);
}

unsigned int hashIndex(const char* data, size_t size) {
    unsigned int index = 0;

    for (unsigned int i = 0; i < size; ++i) {
        index = (index ^ data[i]) << 1;
    }

    return index;
}