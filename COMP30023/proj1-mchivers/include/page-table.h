/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    page-table.h :
            = the interface for page table module.
--------------------------------------------------------------------------- */

#ifndef _PAGE_TABLE_H_
#define _PAGE_TABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "system-specs.h"

#define PAGETABLE_INVALID_FRAME UINT32_MAX
#define PAGE_FAULT_FLAG 1
#define NO_PAGE_FAULT_FLAG 0
#define PAGETABLE_LOOKUP_FAIL -1

/* --- FORWARD DECLARATIONS --- */
typedef struct pagetable_entry pagetable_entry_t;
typedef pagetable_entry_t* pagetable_t;  /* Implemented as an array of pagetable entries */

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
int pagetable_lookup(pagetable_t pt, uint32_t page_number);
void pagetable_set(pagetable_t pt, uint32_t page_number, uint32_t frame_number);
void pagetable_destroy(pagetable_t pt);
pagetable_t pagetable_init(void);

#endif

 