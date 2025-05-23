/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    page-table.c :
            = the page table, keeping track of page-frame pair allocation.
--------------------------------------------------------------------------- */
#include "page-table.h"

#define PAGE_VALID_MASK 1  // Used for bitwise operations

/* --- INTERNAL FUNCTION PROTOTYPES --- */
void set_present_bit(pagetable_entry_t *entry);
void set_absent_bit(pagetable_entry_t *entry);
int page_is_present(pagetable_entry_t *entry);

/* --- DEFINING STRUCTS --- */
struct pagetable_entry {
    uint32_t frame_number;
    uint8_t present;  /* Present(1)/absent(0) bit */
};


/* --- FUNCTION IMPLEMENTATIONS --- */

/** set_present_bit - Set the present bit to 1 (present)
 */
void set_present_bit(pagetable_entry_t *entry) {
    assert(entry != NULL);
    // Set present bit to 1 (bitwise OR)
    entry->present |= PAGE_VALID_MASK;
}

/** set_absent_bit - Set the present bit to 0 (absent)
 */
void set_absent_bit(pagetable_entry_t *entry) {
    assert(entry != NULL);
    // Set present bit to 0 (bitwise AND with the negation of PAGE_VALID_MASK)
    entry->present &= ~PAGE_VALID_MASK;
}

/** pagetable_is_present - given a page table entry, it checks if the page is present.
 *  Returns:
 *    - 0: if the page is NOT present .
 *    - 1: if the page is present.
 */
int page_is_present(pagetable_entry_t *entry) {
    assert(entry != NULL);
    return (entry->present & PAGE_VALID_MASK) != 0;
}

/** pagetable_init - mallocs an appropriately sized page table.
 *    - page table size is determined by system-specs.
 *  Returns:
 *    - A pagetable_t which is used to refrence into page table entries. 
 */
pagetable_t pagetable_init(void) {
    pagetable_t pt = (pagetable_t)calloc(NUMBER_OF_PAGES, sizeof(pagetable_entry_t));
    assert(pt != NULL);
    return pt;
}

/** pagetable_destroy - frees all of the memory allocated.
 */
void pagetable_destroy(pagetable_t pt) { free(pt); }

/** pagetable_lookup - retrieves the frame number for the given page.
 *   - The way the system-specs are designed, it is not possible for the page number
 *     to be greater than the indexing limit
 * Parameters: 
 *    pt - the apge table we are indexing into.
 *    page_number - the page number we index to.
 * Returns:
 *    -1 - representing the page not being allocated to a frame.
 *    the frame number (which can be represented in uint32_t type).
 */
int pagetable_lookup(pagetable_t pt, uint32_t page_number) {
    assert(pt != NULL);
    pagetable_entry_t *entry = &pt[page_number];

    // Check if the page is present (ie has a frame)
    if (page_is_present(entry)) {
        return entry->frame_number;
    } else {  // Page is NOT present
        return PAGETABLE_LOOKUP_FAIL;  // indicates the page is not present
    }
}

/** pagetable_set - sets the frame number for a given page in the page table, updating the 
 *                  corresponding entry with the provided frame number, and the present bit. 
 */
void pagetable_set(pagetable_t pt, uint32_t page_number, uint32_t frame_number) {
    assert(pt != NULL);

    // Access the page table entry directly
    pagetable_entry_t *entry = &pt[page_number];  

    // Set the frame number
    entry->frame_number = frame_number;

    // Mark the page as present 
    set_present_bit(entry);
}