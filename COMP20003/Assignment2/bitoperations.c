/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    bitoperations.c :
            = the implementation of bit operations for radix trie.
--------------------------------------------------------------------------- */

#include "bitoperations.h"

/* Helper function. Gets the bit at bitIndex from the string s. */
int getBit(char *s, unsigned int bitIndex){
    assert(s && bitIndex >= 0);
    unsigned int byte = bitIndex / BITS_PER_BYTE;
    unsigned int indexFromLeft = bitIndex % BITS_PER_BYTE;
    unsigned int offset = (BITS_PER_BYTE - (indexFromLeft) - 1) % BITS_PER_BYTE;
    unsigned char byteOfInterest = s[byte];
    unsigned int offsetMask = (1 << offset);
    unsigned int maskedByte = (byteOfInterest & offsetMask);
    unsigned int bitOnly = maskedByte >> offset;
    return bitOnly;
}

/* Allocates new memory to hold the numBits specified and fills the allocated
    memory with the numBits specified starting from the startBit of the oldKey
    array of bytes. */
char *createStem(char *oldKey, unsigned int startBit, unsigned int numBits){
    assert(numBits > 0 && startBit >= 0 && oldKey);
    int extraBytes = 0;
    if((numBits % BITS_PER_BYTE) > 0){
        extraBytes = 1;
    }
    int totalBytes = (numBits / BITS_PER_BYTE) + extraBytes;
    char *newStem = malloc(sizeof(char) * totalBytes);
    assert(newStem);
    for(unsigned int i = 0; i < totalBytes; i++){
        newStem[i] = 0;
    }
    for(unsigned int i = 0; i < numBits; i++){
        unsigned int indexFromLeft = i % BITS_PER_BYTE;
        unsigned int offset = (BITS_PER_BYTE - indexFromLeft - 1) % BITS_PER_BYTE;
        unsigned int bitMaskForPosition = 1 << offset;
        unsigned int bitValueAtPosition = getBit(oldKey, startBit + i);
        unsigned int byteInNewStem = i / BITS_PER_BYTE;
        newStem[byteInNewStem] |= bitMaskForPosition * bitValueAtPosition;
    }
    return newStem;
}

/* Returns the number of bits within the string from a bit position */
int getNumberOfBits(char *s, unsigned int s_bit_pos) {
    unsigned int totalBits = (strlen(s) + 1) * BITS_PER_BYTE;
    assert(s_bit_pos < totalBits);
    return totalBits - s_bit_pos;
}

/* Combines two prefixes together and returns a newly malloced prefix */
char *combinePrefixes(char *prefix1, unsigned int len1, char *prefix2, unsigned int len2, unsigned int *combined_len) {
    *combined_len = len1 + len2;
    unsigned int combinedLengthBytes = (*combined_len + 7) / 8;
    char *combinedPrefix = (char *)calloc(combinedLengthBytes, sizeof(char));
    assert(combinedPrefix);

    for (unsigned int i = 0; i < len1; i++) {
        int bit = getBit(prefix1, i);
        if (bit) {
            unsigned int byteIndex = i / BITS_PER_BYTE;
            unsigned int bitIndex = i % BITS_PER_BYTE;
            combinedPrefix[byteIndex] |= (1 << (BITS_PER_BYTE - bitIndex - 1));
        }
    }
    for (unsigned int i = 0; i < len2; i++) {
        int bit = getBit(prefix2, i);
        if (bit) {
            unsigned int combinedBitIndex = len1 + i;
            unsigned int byteIndex = combinedBitIndex / BITS_PER_BYTE;
            unsigned int bitIndex = combinedBitIndex % BITS_PER_BYTE;
            combinedPrefix[byteIndex] |= (1 << (BITS_PER_BYTE - bitIndex - 1));
        }
    }

    return combinedPrefix;
}
