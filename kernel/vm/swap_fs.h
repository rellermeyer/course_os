#ifndef __VM_SWAP_FS_H
#define __VM_SWAP_FS_H

#include "file.h"
#include "bitvector.h"
#include "vm.h"

static bit_vector *free_bitvector;

/* swapfs_init initializes swap file system  framework for 'npages' number of pages
 *
 * Returns: -1 if failure, nbytes (>0) written on success
 */
int swapfs_init(int npages);

//Returns: -1 if failure, 1 if success
int swapfs_disable();

/* swapfs_store will store a page to media - not main memory - storage, e.g. HDD
 * from the page*
 * void* page -> data to be paged
 * os_size_t pagesize: The size of a page in bytes, i.e. 4096 b
 * uint32_t* ID -> The page ID or address
 *
 * Returns: The stored page's swap space ID, index, 0 on failure
 */
int swapfs_store(void *page, uint32_t *id);

/* swapfs_retrieve will retrieve the page identified the ID pointer and
 * will store it back into main memory (specified by the void *page pointer)
 *
 * Returns: The retrieved page's memory ID/Address, 0 on failure
 */
int swapfs_retrieve(void *page, uint32_t *id);

#endif
