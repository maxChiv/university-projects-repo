/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    system-specs.h :
            = the global variables of the systems
--------------------------------------------------------------------------- */
// Guard
#ifndef _SPECS_H_
#define _SPECS_H_

#define TRUE 1
#define FALSE 0

#define STATS_FLAG TRUE  /* A flag for stats to be used or not: TRUE - On, FALSE - Off */

/* --- Logical Address Space Specifications --- */
#define BITS_PER_CALL                    32 /* Number of bits for a logical address cal (some can be masked bits)*/
#define LOGICAL_ADDRESS_SPACE_SIZE_EXP   22 /* Exponential value of Logical address space size ie 2 to the exponential_val */
#define LOGICAL_ADDRESS_SPACE_SIZE       (1 << LOGICAL_ADDRESS_SPACE_SIZE_EXP)  /* 2 to the 22 bytes, 4MB */
#define PAGE_SIZE_EXP                    12  /* Exponential value of Page size ie 2 to the exponential_val */
#define PAGE_SIZE                        (1 << PAGE_SIZE_EXP)  /* 2 to the 12 bytes (4KB) */
#define NUMBER_OF_PAGES                  (1 << (LOGICAL_ADDRESS_SPACE_SIZE_EXP - PAGE_SIZE_EXP))  /* Derived number of pages */

    /* Additional derived constants */
#define ADDRESS_BITS                 LOGICAL_ADDRESS_SPACE_SIZE_EXP  /* Number of bits in logical address */
#define PAGE_OFFSET_BITS             PAGE_SIZE_EXP  /* Number of bits for page offset */
#define PAGE_NUMBER_BITS             (LOGICAL_ADDRESS_SPACE_SIZE_EXP - PAGE_SIZE_EXP)  /* Number of bits for page number */

/* --- Physical Address Space Specifications --- */
#define PHYSICAL_ADDRESS_SPACE_SIZE_EXP  20 /* Exponential value of Physical address space size ie 2 to the exponential_val */
#define PHYSICAL_ADDRESS_SPACE_SIZE      (1 << LOGICAL_ADDRESS_SPACE_SIZE_EXP)  /* 2 to the 20 bytes, 1MB */
#define FRAME_SIZE_EXP                   PAGE_SIZE_EXP  /* Same as page */
#define FRAME_SIZE                       PAGE_SIZE      /* Same as page */
#define NUMBER_OF_FRAMES_EXP             8 /* Exponential value of number of pages ie 2 to the exponential_val */
#define NUMBER_OF_FRAMES                 (1 << (NUMBER_OF_FRAMES_EXP))  /* Number of frames */

/* --- For Translation Lookaside Buffer (TLB) --- */
#define TLB_NUMBER_OF_ENTRIES           32 /* Number of entries for the tlb */

#endif /* MY_HEADER_H */
