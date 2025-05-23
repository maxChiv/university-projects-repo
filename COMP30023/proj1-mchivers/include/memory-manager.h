/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    memory-manager.h :
            = the interface for memory-manager.
--------------------------------------------------------------------------- */

// Guard
#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "logical-address-parser.h"
#include "page-table.h"
#include "linked-list/linkedlist.h"
#include "tlb.h"

#if STATS_FLAG
#include "stats.h"
#endif

#define MM_NULL_INT_FLAG -1

/* --- FORWARD DECLARATIONS --- */
typedef struct memory_manager memory_manager_t;

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
memory_manager_t *memory_manager_create(void);
memory_manager_t *memory_manager_create_task_specific(int task);
void memory_manager_destroy(memory_manager_t *mm);
void handle_logical_address(memory_manager_t *mm, uint32_t logical_address, int task);

#endif