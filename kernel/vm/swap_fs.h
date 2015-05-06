#ifndef __VM_SWAP_FS_H
#define __VM_SWAP_FS_H

#include "file.h"
#include "bitvector.h"
#include "vm.h"
#include "klibc.h"
#include "swap_framework.h"
#include <stdio.h>


bit_vector *bv_arr[SWAP_SPACES];
//bit_vector *free_bitvector;


/* swapfs_init initializes swap file system  framework for 'npages' number of pages
 *
 * Returns: -1 if failure, number of bytes written on success
 */
int32_t swapfs_init(int, int);


//Returns: -1 if failure, 1 if success
int32_t swapfs_disable(int);


/* swapfs_store will store a page to media - not main memory - storage, e.g. HDD
 * from the page*
 * void* page -> data to be paged
 * os_size_t pagesize: The size of a page in bytes, i.e. 4096 b
 * uint32_t* ID -> The page ID or address
 *
 * Returns: The stored page's swap space ID, index, -1 on failure
 */
int32_t swapfs_store(void *, uint32_t *, int);


/* swapfs_retrieve will retrieve the page identified the ID pointer and
 * will store it back into main memory (specified by the void *page pointer)
 *
 * Returns: The retrieved page's memory ID/Address, -1 on failure
 */
int32_t swapfs_retrieve(void *, uint32_t *);

#endif
