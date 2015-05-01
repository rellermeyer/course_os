#ifndef __VM_SWAP_FS_H
#define __VM_SWAP_FS_H

#include "file.h"
#include "bitvector.h"
#include "vm.h"

static bit_vector *free_bitvector;


int swapfs_init(int npages);


int swapfs_store(void *page, uint32_t *id);


int swapfs_receive(void *page, uint32_t *id);

#endif
