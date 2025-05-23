/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    logical-address-parser.h :
            = the interface for logical-address-parser.c.
--------------------------------------------------------------------------- */
// Guard
#ifndef _LOGICAL_ADDRESS_PARSER_H_
#define _LOGICAL_ADDRESS_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "system-specs.h"

/* Masks for address translation */
#define PAGE_OFFSET_MASK             (PAGE_SIZE - 1)  /* Masked bits for page offset to then use with bitwise ops */
#define PAGE_NUMBER_MASK             ((NUMBER_OF_PAGES - 1) << PAGE_OFFSET_BITS)  /* Masked bits for page number to then use with bitwise ops */

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
uint32_t get_unmasked_logical_address(uint32_t logical_address);
uint32_t get_page_number(uint32_t logical_address);
uint32_t get_offset(uint32_t logical_address);

#endif