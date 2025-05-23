/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    memory-manager.c :
            = the memory manager, calling all tasks and using other files.
--------------------------------------------------------------------------- */
#include "memory-manager.h"

/* --- INTERNAL FUNCTION PROTOTYPES --- */
uint32_t get_physical_address(uint32_t frame_number, uint32_t offset);
int get_free_frame(memory_manager_t* mm);
void evict_a_page(memory_manager_t* mm);
void load_page(memory_manager_t *mm, uint32_t page_number, uint32_t frame_number);

// --- For handling logical address
void parse_logical_address(uint32_t logical_address, uint32_t *page_number, uint32_t *offset);
int handle_tlb_lookup(tlb_t *tlb, uint32_t page_number, uint32_t offset, uint32_t *frame_number);
uint32_t resolve_frame_for_page(memory_manager_t *mm, uint32_t page_number);
int handle_page_table_lookup(memory_manager_t *mm, uint32_t page_number, uint32_t *frame_number);
void handle_update_tlb(tlb_t* tlb, uint32_t page_number, uint32_t frame_number);

/* --- DEFINING STRUCTS --- */
struct memory_manager {
    pagetable_t pagetable;      /* The page table of memory manager. */
    list_t *free_frames_list;   /** The free frames list, storing the number of the free frame.
                                 *  - Data is stored as void*, so cast it to an integer when recieving.
                                 *  - Treated as a FIFO, so 'append()' to tail, 'fifo_pop_head()' from head.
                                 */
    list_t *page_queue;         /** The page queue, storing the FIFO algorithm evicting data structure needed. 
                                 *  - Data is stored as void*, so cast it to an integer when recieving.
                                 *  - Treated as a FIFO, so 'append()' to tail, 'fifo_pop_head()' from head.
                                 */
    tlb_t *tlb;
};


/* --- FUNCTION IMPLEMENTATIONS --- */

/** memory_manager_create - Allocates and initialises a memory manager instance.
 *   This sets up all internal components like the page table.
 * 
 * Returns:
 *   A pointer to an initialised memory_manager_t.
 */
memory_manager_t *memory_manager_create(void) {
    memory_manager_t *mm = malloc(sizeof(memory_manager_t));
    assert(mm != NULL);

    mm->pagetable = pagetable_init();

    // Create the list for storing free frames, and add in the frames.
    mm->free_frames_list = list_create();
    for (uint32_t i = 0; i < NUMBER_OF_FRAMES; i++) {
        append(mm->free_frames_list, i);
    }

    // Create the page queue.
    mm->page_queue = list_create();

    // Create the tlb
    mm->tlb = tlb_create();

    return mm;
}

/** memory_manager_create_task_specific - Allocates and initialises a memory 
 * manager instance, with specific components intialised, depnding on implementation. 
 *      This sets up all internal components like the page table.
 * 
 * Returns:
 *   A pointer to an initialised memory_manager_t.
 */
memory_manager_t *memory_manager_create_task_specific(int task) {
    memory_manager_t *mm = memory_manager_create();

    // In this implementation, only tlb is not used for specific tasks.
    if (task < 4) {
        tlb_destroy(mm->tlb);
        mm->tlb = NULL;
    }
    return mm;
}


/** memory_manager_destroy - Frees all resources used by the memory manager.
 *   Cleans up internal components such as the page table.
 * 
 * Parameters:
 *   mm - Pointer to the memory manager to destroy.
 */
void memory_manager_destroy(memory_manager_t *mm) {
    if (!mm) return;
    pagetable_destroy(mm->pagetable);
    free_list(mm->free_frames_list);
    free_list(mm->page_queue);

    // Dependent on initialisation.
    if (mm->tlb != NULL) {
        tlb_destroy(mm->tlb);
    }
    free(mm);
}

/** get_free_frame - Retrieves a free frame from the memory manager.
 * 
 * If the list of free frames is not empty, this function removes and returns
 * the next available frame using FIFO order see 'evict_a_page'. If no free frames 
 * remain, it returns -1 to indicate that a frame replacement policy should be invoked.
 *
 * Parameters:
 *   - mm: A pointer to the memory manager.
 * Returns:
 *   - A valid frame number if one is available.
 *   - -1 if no free frames are available.
 */
int get_free_frame(memory_manager_t* mm) {
    assert(mm != NULL);
    assert(mm->free_frames_list != NULL);

    if (is_empty_list(mm->free_frames_list)) {
        // There are no free frames.
        evict_a_page(mm);
    } 
    return fifo_pop_head(mm->free_frames_list);
}

/** evict_a_page - removes the oldest page from memory (FIFO policy)
 *  Parameters:
 *      mm - the memory manager containing the page queue and page table
 *  
 *  This function evicts the page at the front of the FIFO queue, marks its
 *  page table entry as invalid, and returns its frame to the free frame list.
 */
void evict_a_page(memory_manager_t* mm) {
    uint32_t evicted_page_number = (uint32_t)fifo_pop_head(mm->page_queue);
    uint32_t frame_number = pagetable_lookup(mm->pagetable, evicted_page_number);

    // Update the page table, ie. change the frame associated to the evicted page
    pagetable_set(mm->pagetable, evicted_page_number, PAGETABLE_INVALID_FRAME);

    append(mm->free_frames_list, frame_number);
}

/** load_page - loads a virtual page into the specified physical frame
 *
 *  Updates the page table to map the page to the frame and appends the page to
 *  the FIFO queue for tracking page usage.
 *  Parameters:
 *      mm - pointer to the memory manager
 *      page_number - the virtual page number to be loaded
 *      frame_number - the physical frame number to load the page into
 */
void load_page(memory_manager_t *mm, uint32_t page_number, uint32_t frame_number) {
    pagetable_set(mm->pagetable, page_number, frame_number);
    append(mm->page_queue, page_number);
}

/** get_physical_address - returns the constructed physical address
 *  Parameters:
 *      frame_number - the frame number for the logical address
 *      offset - the offset for the logical address
 */
 uint32_t get_physical_address(uint32_t frame_number, uint32_t offset) {
    return (frame_number * FRAME_SIZE) + offset;
 }

/** parse_logical_address - Extracts the page number and offset from a logical address.
 * 
 * If STATS_FLAG is enabled, the function also logs the parsed values for analysis.
 *
 * Parameters:
 *   logical_address - The 32-bit logical address to parse.
 *   page_number - Pointer to a uint32_t variable where the page number will be stored.
 *   offset - Pointer to a uint32_t variable where the offset will be stored.
 */
 void parse_logical_address(uint32_t logical_address, uint32_t *page_number, uint32_t *offset) {
    *page_number = get_page_number(logical_address);
    *offset = get_offset(logical_address);

    #if STATS_FLAG  // Task 1 logging
    log_task1(logical_address, *page_number, *offset);
    #endif
 }


/** handle_tlb_lookup - Handles a TLB lookup and logs the result.
 *
 * Parameters:
 *   tlb - Pointer to the TLB structure.
 *   page_number - The virtual page number to look up.
 *   offset - The offset within the page (used only for logging).
 *   frame_number - Output pointer to store the frame number on a TLB hit.
 *
 * Returns:
 *   1 if TLB hit (frame_number is set), -1 if TLB miss.
 */
 int handle_tlb_lookup(tlb_t *tlb, uint32_t page_number, uint32_t offset, uint32_t *frame_number) {
    int tmp = tlb_lookup(tlb, page_number);

    if (tmp == TLB_MISS) {
        #if STATS_FLAG
        log_task4a(STATS_TLB_MISS, page_number, STATS_NONE_INT, STATS_NONE_INT);
        #endif
        return TLB_MISS;  // TLB miss
    } else {
        *frame_number = tmp;
        uint32_t physical_address = get_physical_address(*frame_number, offset);

        #if STATS_FLAG
        log_task4a(STATS_TLB_HIT, page_number, *frame_number, physical_address);
        #endif
        return TLB_HIT;  // TLB hit
    }
}

/** resolve_frame_for_page - Allocates a frame for the given page, evicting if needed.
 * Parameters:
 *   mm - The memory manager
 *   page_number - The logical page number to load
 *   evicted_page_number - Optional output for stats (set to -1 if nothing evicted)
 *
 * Returns:
 *   The allocated frame number
 */
 uint32_t resolve_frame_for_page(memory_manager_t *mm, uint32_t page_number) {
    uint32_t evicted_page_number = MM_NULL_INT_FLAG;

    // Check if eviction is necessary
    if (is_empty_list(mm->free_frames_list)) {
        evicted_page_number = list_get_head(mm->page_queue);  // Don't remove yet
    }

    // Allocate frame
    uint32_t frame_number = get_free_frame(mm);

    #if STATS_FLAG  // Task 3 logging
    if (evicted_page_number != MM_NULL_INT_FLAG) {  // Using flag from before.
        log_task3(evicted_page_number, frame_number);
    }
    #endif

    // Flush TLB if needed
    if (mm->tlb != NULL && evicted_page_number != MM_NULL_INT_FLAG) {
        int tlb_flush_res = tlb_flush_entry(mm->tlb, evicted_page_number);

        #if STATS_FLAG
        if (tlb_flush_res != TLB_FAIL) {
            log_task4b(evicted_page_number, tlb_get_size(mm->tlb));
        }
        #endif
    }

    // Load the new page
    load_page(mm, page_number, frame_number);

    return frame_number;
}

/** handle_page_table_lookup - Looks up a page number in the page table, and updates the TLB if needed.
 *
 * This function performs a lookup of the page number in the memory manager’s page table.
 * If the page is not present (page fault), it:
 *   - Retrieves a free frame (and evicts an old page if necessary),
 *   - Loads the required page into memory,
 *   - Flushes the TLB entry for any evicted page (if a TLB exists).
 *
 * After a successful page table lookup (or page load), the TLB is updated with the new entry,
 * if the TLB is enabled. Also logs task-specific statistics.
 *
 * Parameters:
 *   mm           - Pointer to the memory manager.
 *   page_number  - Page number to look up.
 *   frame_number - Output parameter for the resulting frame number.
 *   offset       - Offset used to calculate physical address for logging.
 *
 * Returns:
 *   PAGE_FAULT_FLAG or NO_PAGE_FAULT_FLAG.
 */
int handle_page_table_lookup(memory_manager_t *mm, uint32_t page_number, uint32_t *frame_number) {
    uint8_t page_fault_flag;

    // TASK 2, and TASK 3.
    int tmp;  // Temporary placeholder for lookup
    // Use page number to identify relative page table entry 
    if ((tmp = pagetable_lookup(mm->pagetable, page_number)) == PAGETABLE_LOOKUP_FAIL) {
        // PAGE FAULT (ie. page not present in physical memeory / no frame.)
        page_fault_flag = PAGE_FAULT_FLAG;
        *frame_number = resolve_frame_for_page(mm, page_number);

    } else {
        // Recieved the frame number for that page
        page_fault_flag = NO_PAGE_FAULT_FLAG;
        *frame_number = tmp;
    }

    if (mm->tlb != NULL) {
        // TASK 4, after page table, and a TLB-Miss.
        handle_update_tlb(mm->tlb, page_number, *frame_number);
    }
    return page_fault_flag;
}

/** handle_update_tlb - a wrapper to hide all the unnecassary checking revolved around the tlb design (this was to be able to do logging)
 * precodnitions is this is run directly after the page table has run to completion.
 */
 void handle_update_tlb(tlb_t* tlb, uint32_t page_number, uint32_t frame_number) {
    // Attempt to add the new tlb entry 
    if (tlb_add(tlb, page_number, frame_number) != TLB_FAIL) {
        // Successfully added the page
        #if STATS_FLAG
        log_task4c(STATS_NONE_INT, page_number);
        #endif
        return;
    }
    // Did not successfully add page.
    // Flush a page to make room.
    uint32_t flushed_page_number = tlb_flush_entry(tlb, tlb_get_LRU_page(tlb));
    tlb_add(tlb, page_number, frame_number);
    #if STATS_FLAG
    log_task4c(flushed_page_number, page_number);
    #endif
 }

/** handle_logical_address - Translates a logical address into a physical address.
 *
 * - Checks if the logical address is in the TLB.
 * - Checks if the logical address is in the Page Table.
 * - Updates TLB
 * - Uses Information to establish Physical Address
 * - All modularised functions only take pointers to variables they can find/initialise,
 *   otherwise, they are passed through as normal integers.
 *
 * Parameters:
 *   mm - the memory manager data struct
 *   logical_address - The logical (virtual) address to translate.
 *   task - the task number wanted to complete (Assignment specific variable)
 */
void handle_logical_address(memory_manager_t *mm, uint32_t logical_address, int task) {
    // Variables for handling logical address
    uint32_t page_number;
    uint32_t offset;
    uint32_t frame_number;
    uint32_t physical_address;
    
    // Task Dependent execution
    if (task >= 1) {
        // TASK 1
        parse_logical_address(logical_address, &page_number, &offset);
    }

    if (task >= 4) {
        // TLB Lookup 
        if (handle_tlb_lookup(mm->tlb, page_number, offset, &frame_number) == TLB_HIT) {
            // TLB HIT, no need to go further
            return;
        }
    }

    if (task >= 2) {
        // Recieve frame number.
        int page_fault_flag = handle_page_table_lookup(mm, page_number, &frame_number);
        // Find physical address
        physical_address = get_physical_address(frame_number, offset);

        #if STATS_FLAG  // Task 2 logging
        log_task2(page_number, page_fault_flag, frame_number, physical_address);
        #endif
    }
}
