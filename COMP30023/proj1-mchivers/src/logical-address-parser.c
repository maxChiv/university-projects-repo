/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    logical-address-parser.c :
            = the module to handle all the logical address association.
--------------------------------------------------------------------------- */

#include "logical-address-parser.h"

/* --- FUNCTION IMPLEMENTATIONS --- */

/** get_unmasked_logical_address - Retrieves the logical address without the mask bits. 
 */
uint32_t get_unmasked_logical_address(uint32_t logical_address) {
    // Combine the two DEFINED masks to retain only the page number and offset bits
    return (logical_address & (PAGE_NUMBER_MASK | PAGE_OFFSET_MASK));
}

/** get_page_number - Retrieves the logical address's page number.
 */
uint32_t get_page_number(uint32_t logical_address) {
    // Uses mask to find the page number, then moves it back to 
    // start of int (ie where page offset is).
    return (logical_address & PAGE_NUMBER_MASK) >> PAGE_OFFSET_BITS;
}

/** get_offset - Retrieves the logical address's offset.
 */
uint32_t get_offset(uint32_t logical_address) { return logical_address & PAGE_OFFSET_MASK; }

