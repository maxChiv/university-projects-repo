/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    bitoperations.h :
            = the interface module for bit operations used in the radix trie.
--------------------------------------------------------------------------- */

// Guard
#ifndef _BIT_OPERATIONS_H_
#define _BIT_OPERATIONS_H_

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define BITS_PER_BYTE 8
#define IDENTICAL_BIT_CMP -1

int getBit(char *s, unsigned int bitIndex);
char *createStem(char *oldKey, unsigned int startBit, unsigned int numBits);
int getNumberOfBits(char *s, unsigned int s_bit_pos);
char *combinePrefixes(char *prefix1, unsigned int len1, char *prefix2, unsigned int len2, unsigned int *combined_len);


#endif
