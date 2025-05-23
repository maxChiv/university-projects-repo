/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    tlb.h :
            = the interface for translation lookaside buffer (tlb).
--------------------------------------------------------------------------- */
#ifndef _TLB_H_
#define _TLB_H_

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "system-specs.h"
#include "linked-list/generic-doubly-linkedlist.h"
#include "linked-list/linkedlist.h"

#define TLB_FAIL -1
#define TLB_SUCCESS 1
#define TLB_MISS -1
#define TLB_HIT 1

/* --- FORWARD DECLARATIONS --- */
typedef struct tlb_entry tlb_entry_t;
typedef struct tlb tlb_t;
typedef struct tlb_hash_entry tlb_hash_entry_t;

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
tlb_t* tlb_create(void);
void tlb_destroy(tlb_t *tlb);
int tlb_lookup(tlb_t* tlb, uint32_t page_number);
int tlb_add(tlb_t* tlb, uint32_t page_number, uint32_t frame_number);
int32_t tlb_flush_entry(tlb_t *tlb, uint32_t page_number);
uint32_t tlb_get_LRU_page(tlb_t *tlb);
uint32_t tlb_get_size(tlb_t *tlb);

#endif