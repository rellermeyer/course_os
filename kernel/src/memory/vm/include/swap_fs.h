#ifndef __VM_SWAP_FS_H
#define __VM_SWAP_FS_H

#include "bitvector.h"
#include "swap_framework.h"


/* Last Update: 05/11/15 */


bit_vector *bv_arr[SWAP_SPACES];
// bit_vector *free_bitvector;


//helper function
char *generate_filename(uint8_t);


/* swapfs_init initializes swap file system  framework for 'npages' number of pages
 *
 * Returns: -1 if failure, number of bytes written on success
 */
int32_t swapfs_init(int, uint8_t);


/* swapfs_store will store a page to media - not main memory - storage, e.g. HDD
 * from the page*
 * void* page -> data to be paged
 * os_size_t pagesize: The size of a page in bytes, i.e. 4096 b
 * uint32_t* ID -> The page ID or address
 *
 * Returns: The stored page's swap space ID, index OR -1 on failure
 * NOTE: Relying on the changed id pointer is safer than using the output (unless 
 * for error checking) due to the int64_t type output
 */
int64_t swapfs_store(void*, uint32_t*, uint8_t);


/* swapfs_retrieve will retrieve the page identified by the ID pointer and
 * will store it back into main memory (specified by the void *page pointer)
 *
 * Returns: -1 on failure and 1 on success
 */
int64_t swapfs_retrieve(void*, uint32_t*, uint8_t);


/* Returns: -1 if failure, 1 if success */
int32_t swapfs_disable(uint8_t);


#endif
