/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    stats.c :
            = the stats module for logging the program at different stages

NOTES:
	When you add a new log (eg. LOG_TASK5), you:
	•	Add a new enum value for this task eg (LOG_TASK5)
	•	Add a new struct for task 5’s data.
	•	Add it to the union in log_entry_t.
	•	Add a case LOG_TASK5 in log_event().
    •	Create the function for this log to be called (ie log_task5) and add to header file.

    Log entries can be stored in a later versions.
--------------------------------------------------------------------------- */
#include "stats.h"

/* --- INTERNAL FUNCTION PROTOTYPES --- */
void log_event(const log_entry_t *entry);

/* --- DEFINING STRUCTS --- */
/* Define all the other log structs for specific tasks */
// Task 1
typedef struct {
    uint32_t logical_address;
    uint32_t page_number;
    uint32_t offset;
} log_task1_data_t;
// Task 2
typedef struct {
    uint32_t page_number; 
    uint8_t page_fault; 
    uint32_t frame_number;
    uint32_t physical_address;
} log_task2_data_t;
// Task 3
typedef struct {
    uint32_t evicted_page_number;
    uint32_t freed_frame_number;
} log_task3_data_t;
// Task 4
typedef struct {
    uint8_t tlb_hit;
    uint32_t page_number;
    int frame_number;
    int physical_address;
} log_task4a_data_t;
typedef struct {
    uint32_t flushed_page_number;
    uint32_t tlb_size;
} log_task4b_data_t;
typedef struct {
    int tlb_removed_page_number;
    uint32_t tlb_add_page_number;
} log_task4c_data_t;

struct log_entry {
	log_entry_type_t type;
    union {
        log_task1_data_t task1;
        log_task2_data_t task2;
        log_task3_data_t task3;
        log_task4a_data_t task4a;
        log_task4b_data_t task4b;
        log_task4c_data_t task4c;
        // ...add other tasks if needed.
    } details;
};

/* --- FUNCTION IMPLEMENTATIONS --- */

/** log_event - takes a log entry and logs it by printing it in the specified format. 
 *
 *  - Determines how to log the event by its enum log_event_type_t
 * Parameters: 
 *  - A log entry with its given details completed as determined by the log event type
 * Returns:
 *  - It is a void function, however it will be printing to stdout.
 */
void log_event(const log_entry_t *entry) {
    assert(entry != NULL);
    assert(entry->type >= 0 && entry->type < LOG_TASK_COUNT);

    switch (entry->type) {
        case LOG_TASK1:
            printf("logical-address=%u,page-number=%u,offset=%u\n",
                entry->details.task1.logical_address,
                entry->details.task1.page_number,
                entry->details.task1.offset);
            break;
        case LOG_TASK2:
            printf("page-number=%u,page-fault=%d,frame-number=%u,physical-address=%u\n",
                entry->details.task2.page_number,
                entry->details.task2.page_fault,
                entry->details.task2.frame_number,
                entry->details.task2.physical_address);
            break;
        case LOG_TASK3:
            printf("evicted-page=%u,freed-frame=%u\n",
                entry->details.task3.evicted_page_number,
                entry->details.task3.freed_frame_number);
            break;
        case LOG_TASK4A:
            if (entry->details.task4a.frame_number != STATS_NONE_INT) {
                printf("tlb-hit=%u,page-number=%u,frame=%d,physical-address=%d\n",
                    entry->details.task4a.tlb_hit,
                    entry->details.task4a.page_number,
                    entry->details.task4a.frame_number,
                    entry->details.task4a.physical_address);
            } else {
                printf("tlb-hit=%u,page-number=%u,frame=%s,physical-address=%s\n",
                    entry->details.task4a.tlb_hit,
                    entry->details.task4a.page_number,
                    STATS_NONE_STR, STATS_NONE_STR);
            }
            break;
        case LOG_TASK4B:
            printf("tlb-flush=%u,tlb-size=%u\n",
                entry->details.task4b.flushed_page_number,
                entry->details.task4b.tlb_size);
            break;
        case LOG_TASK4C:
            if (entry->details.task4c.tlb_removed_page_number != STATS_NONE_INT) {
                printf("tlb-remove=%u,tlb-add=%u\n",
                    entry->details.task4c.tlb_removed_page_number,
                    entry->details.task4c.tlb_add_page_number);
            } else {
                printf("tlb-remove=%s,tlb-add=%u\n",
                    STATS_NONE_STR,
                    entry->details.task4c.tlb_add_page_number);
            }
            break;
        // Future log types can be handled here
        default:
            printf("Unknown Log Type\n");
            break;
    }
}

/** log_task1 - takes the data and uses it to intialise a log entry and then log it to stdout.
 */
void log_task1(uint32_t logical_address, uint32_t page_number, uint32_t offset) {
    log_entry_t entry;
    entry.type = LOG_TASK1;
    // Initialise Variables
    entry.details.task1.logical_address = logical_address;
    entry.details.task1.page_number = page_number;
    entry.details.task1.offset = offset;

    log_event(&entry);
}

/** log_task2 - takes the data and uses it to intialise a log entry and then log it to stdout.
 */
void log_task2(uint32_t page_number, uint8_t page_fault, uint32_t frame_number, uint32_t physical_address) {
    log_entry_t entry;
    entry.type = LOG_TASK2;
    // Initialise Variables
    entry.details.task2.page_number = page_number;
    entry.details.task2.page_fault = page_fault;
    entry.details.task2.frame_number = frame_number;
    entry.details.task2.physical_address = physical_address;

    log_event(&entry);
}

/** log_task3 - takes the data and uses it to intialise a log entry and then log it to stdout.
 */
 void log_task3(uint32_t evicted_page_number, uint32_t freed_frame_number) {
    log_entry_t entry;
    entry.type = LOG_TASK3;
    // Initialise Variables
    entry.details.task3.evicted_page_number = evicted_page_number;
    entry.details.task3.freed_frame_number = freed_frame_number;

    log_event(&entry);
}


/** log_task4a - takes the data and uses it to intialise a log entry and then log it to stdout.
 * - After looking for an entry in the TLB.
 * - Should pass -1 for no frame_number, or no physical_address
 */
void log_task4a(uint8_t tlb_hit, uint32_t page_number, int frame_number, int physical_address) {
    log_entry_t entry;
    entry.type = LOG_TASK4A;

    // Initialise Variables
    entry.details.task4a.tlb_hit = tlb_hit;
    entry.details.task4a.page_number = page_number;
    entry.details.task4a.frame_number = frame_number;
    entry.details.task4a.physical_address = physical_address;
    
    log_event(&entry);
}

/** log_task4b - takes the data and uses it to intialise a log entry and then log it to stdout.
 * - After a TLB fush caused by a page eviction (i.e., after a page present in the TLB is evicted
    and the page is fushed (removed) from the TLB)
 */
 void log_task4b(uint32_t flushed_page_number, uint32_t tlb_size) {
    log_entry_t entry;
    entry.type = LOG_TASK4B;

    // Initialise variables
    entry.details.task4b.flushed_page_number = flushed_page_number;
    entry.details.task4b.tlb_size = tlb_size;

    log_event(&entry);
}

/** log_task4c - takes the data and uses it to intialise a log entry and then log it to stdout.
 * - After the TLB is updated following a TLB miss
 */
void log_task4c(int tlb_removed_page_number, uint32_t tlb_add_page_number) {
    log_entry_t entry;
    entry.type = LOG_TASK4C;

    // Initialise variables
    entry.details.task4c.tlb_removed_page_number = tlb_removed_page_number;
    entry.details.task4c.tlb_add_page_number = tlb_add_page_number;

    log_event(&entry);
}
