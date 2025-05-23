/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    stats.h :
            = the interface for the stats module
--------------------------------------------------------------------------- */
// Guard
#ifndef _STATS_H_
#define _STATS_H_

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "system-specs.h"
    // ^ Enable or disable stats logging globally 

#define STATS_NONE_INT -1
#define STATS_NONE_STR "none"
#define STATS_TLB_HIT 1
#define STATS_TLB_MISS 0

/* --- FORWARD DECLARATIONS ---*/

typedef struct log_entry log_entry_t;

typedef enum {
    LOG_TASK1,  // Logical address parsing
    LOG_TASK2,  // Page table lookup & faults
    LOG_TASK3,  // Page replacement
    LOG_TASK4A,   // TLB operations
    LOG_TASK4B,   // TLB operations
    LOG_TASK4C,   // TLB operations
    LOG_TASK_COUNT  // sentinel, not a real task
} log_entry_type_t;

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
void log_task1(uint32_t logical_address, uint32_t page_number, uint32_t offset);
void log_task2(uint32_t page_number, uint8_t page_fault, uint32_t frame_number, uint32_t physical_address);
void log_task3(uint32_t evicted_page_number, uint32_t freed_frame_number);
void log_task4a(uint8_t tlb_hit, uint32_t page_number, int frame_number, int physical_address);
void log_task4b(uint32_t flushed_page_number, uint32_t tlb_size);
void log_task4c(int tlb_removed_page_number, uint32_t tlb_add_page_number);
    /* Add more log_taskX for intialising the log entry. */

#endif