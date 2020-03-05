#include <pagealloc2.h>
#include <stdio.h>
#include <stdlib.h>

struct PageAllocator allocator;

void pagealloc_init(size_t start, size_t end) {

    // Create the first sliceinfo at the start address
    struct MemorySliceInfo * firstinfo = (struct MemorySliceInfo *)start;
    // This start address is itself a memory slice
    union MemorySlice * firstslice = (union MemorySlice *)firstinfo;

    // Make the first sliceinfo describe this first slice.
    firstinfo->next = NULL;
    // It's type is sliceinfo
    firstinfo->type = SliceInfo;
    // It points to the first slice
    firstinfo->slice = (union MemorySlice *)firstinfo;
    // And there's one sliceinfo struct in it. This first one.
    firstinfo->filled = 1;

    // Now make an allocator with one sliceinfo in the allocated array.
    allocator = (struct PageAllocator){
            .start = start,
            .end = end,
            .l2ptPartialFree = NULL,
            .pagePartialFree = NULL,
            .allocated = firstinfo,
            .unused = NULL,
    };

    // the infoindex is 1 since we just allocated the 0th one above^^^
    size_t infoindex = 1;
    union MemorySlice * currentslice = firstslice;

    // Go through all memory and index it with sliceinfo structs.
    for (size_t i = start; i < end; i += sizeof(union MemorySlice)) {
        struct MemorySliceInfo * currentsliceinfo = &currentslice->sliceinfo[infoindex];

        // make this sliceinfo struct point to the right slice
        currentsliceinfo->slice = (union MemorySlice *) i;

        // Add the sliceinfo to the unused list
        currentsliceinfo->next = allocator.unused;
        allocator.unused = currentsliceinfo;

        // continue to the next slicinfo
        infoindex++;

        if (infoindex >= SLICEINFO_PER_SLICE) {
            // We can now do this as we already made at least one new sliceinfo struct on the unused list
            // Which we can use for this.
            currentslice = allocate_new_sliceinfo_slice()->slice;
        }
    }
}

struct MemorySliceInfo * allocate_new_sliceinfo_slice() {
    // Take a slice from the unused list and the the next one to the top of unused.
    struct MemorySliceInfo * sliceinfo = allocator.unused;
    allocator.unused = sliceinfo->next;

    // Change it's type to typeinfo
    sliceinfo->type = SliceInfo;
    sliceinfo->filled = 0;

    // Add it to the allocated list
    sliceinfo->next = allocator.allocated;
    allocator.allocated = sliceinfo;

    return sliceinfo;
}


int main() {
    size_t size = 128 * 1024 * 1024;
    size_t start = (size_t) malloc(size);

    pagealloc_init(start, start + size);
}
