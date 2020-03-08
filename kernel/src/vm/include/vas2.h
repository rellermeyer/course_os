#ifndef VAS_2_H
#define VAS_2_H

#include "vm2.h"
#include "tlb_cache_id_allocator.h"


struct vas2 {
    struct TLBDescriptor tlbDescriptor;
    struct L1PageTable * l1PageTable;
    // TODO: Do we want to store L2Tables to make freeing VASes faster?
 };


#endif
