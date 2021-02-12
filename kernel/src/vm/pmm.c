#include <constants.h>
#include <hardwareinfo.h>
#include <pmm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Private functions
// Returns the index of the first zero from the LSB
size_t first_free(uint16_t);

// MemorySliceInfo linked list helper functions.
void push_to_ll(struct MemorySliceInfo ** head, struct MemorySliceInfo * entry);
struct MemorySliceInfo * pop_from_ll(struct MemorySliceInfo ** head);
// It's important that for this function you give it the correct head. If it's not it may happen
// That the head you give it will be mixed up with the actual head of the list it was in.
void remove_element_ll(struct MemorySliceInfo ** head, struct MemorySliceInfo * entry);

void pmm_init(size_t start, size_t end) {
    INFO("Building pmm from 0x%x to 0x%x of size 0x%x", start, end, end - start);

    // Create the first sliceinfo at the start address
    struct MemorySliceInfo * firstinfo = (struct MemorySliceInfo *)start;
    // This start address is itself a memory slice
    union MemorySlice * firstslice = (union MemorySlice *)firstinfo;

    // Make the first sliceinfo describe this first slice.
    firstinfo->next = NULL;
    firstinfo->prev = NULL;
    // It's type is sliceinfo
    firstinfo->type = BucketInfo;
    // It points to the first slice
    firstinfo->slice = firstslice;
    // And there's one sliceinfo struct in it. This first one.
    firstinfo->filled = 1;

    // Now make a pmm with one sliceinfo in the allocated array.
    physicalMemoryManager = (struct PhysicalMemoryManager){
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
    // We start 1 Memoryslice after `start` because we already allocated the first one at the start
    // of this function.
    for (union MemorySlice * i = ((union MemorySlice *)start + 1); i < ((union MemorySlice *)end);
         i++) {
        struct MemorySliceInfo * currentsliceinfo = &currentslice->bucketinfo[infoindex];


        // make this sliceinfo struct point to the right slice
        currentsliceinfo->slice = i;

        if (address_in_reserved_region((size_t)i)) {
            // Mark slice as reserved for MMIO
            currentsliceinfo->reserved = 1;
            // The slice is in the MMIO region, should not be used so put it in the
            // allocated list permanently
            push_to_ll(&physicalMemoryManager.allocated, currentsliceinfo);
        } else {
            // Add the sliceinfo to the unused list
            push_to_ll(&physicalMemoryManager.unused, currentsliceinfo);
        }

        // continue to the next sliceinfo
        infoindex++;

        if (infoindex >= SLICEINFO_PER_SLICE) {
            // We can now do this as we already made at least one new sliceinfo struct on the unused
            // list Which we can use for this.
            currentslice = pmm_new_sliceinfo_slice()->slice;
            infoindex = 0;
        }
    }
}

struct MemorySliceInfo * pmm_new_sliceinfo_slice() {
    // Take a slice from the unused list and the the next one to the top of unused.
    struct MemorySliceInfo * sliceinfo = pop_from_ll(&physicalMemoryManager.unused);

    // Change it's type to typeinfo
    sliceinfo->type = BucketInfo;
    sliceinfo->filled = 0;

    // Add it to the allocated list
    push_to_ll(&physicalMemoryManager.allocated, sliceinfo);


    return sliceinfo;
}

struct MemorySliceInfo * pmm_get_sliceinfo_for_slice(union MemorySlice * slice) {
    const int bucketsize = (SLICEINFO_PER_SLICE * sizeof(union MemorySlice));

    // Take the address of the slice and determine in which bucket it falls.
    // each bucket is a sliceinfo struct describing a slice with sliceinfo structs in it.
    // The buckets therefore describes (682 * 16K) blocks.

    size_t offset_from_allocator_start = (size_t)slice - physicalMemoryManager.start;

    // divide by blocksize
    size_t bucketindex = (offset_from_allocator_start) / (bucketsize);

    // We need a small correction factor. Normally the bucketinfo
    // slice for a bucket sits in the *last* slice of the previous bucket.
    // However, for the first bucket, the bucketinfo slice is actually the
    // *first* slice in the bucket
    size_t correction = bucketindex == 0 ? 0 : 1;

    // Multiply by blocksize again (NOTE: this is so we round down! The division and multiplication
    // DO NOT CANCEL OUT) Subtract two as the bucket is actually the last page of the previous
    // bucket.
    union MemorySlice * bucketinfo =
        ((union MemorySlice *)(physicalMemoryManager.start + bucketindex * bucketsize)) -
        correction;

    // The first non-information slice in a bucket is always the slice after the information slice.
    // Except for the first bucket.
    size_t first_address_in_bucket = (size_t)(bucketinfo + correction);

    // now calculate how many times 16kb we are away from this.
    // first_address_in_bucket - offset_from_allocator_start
    size_t index = ((size_t)slice - first_address_in_bucket) / sizeof(union MemorySlice);

    struct MemorySliceInfo * info = &bucketinfo->bucketinfo[index];

    return info;
}

void pmm_free_l1_pagetable(struct L1PageTable * pt) {
    if (pt == NULL) { return; }

    struct MemorySliceInfo * sliceinfo = pmm_get_sliceinfo_for_slice((union MemorySlice *)pt);

    if (sliceinfo->reserved) {
        WARN("Attempted to free reserved slice");
        return;
    }

    remove_element_ll(&physicalMemoryManager.allocated, sliceinfo);

    // now push it on the unused list.
    push_to_ll(&physicalMemoryManager.unused, sliceinfo);

    // Since it's the first thing on the unused list, make prev null.
    sliceinfo->prev = NULL;
}

struct L1PageTable * pmm_allocate_l1_pagetable() {
    if (physicalMemoryManager.unused == NULL) { return NULL; }

    // Take a slice from the unused list.
    struct MemorySliceInfo * sliceinfo = pop_from_ll(&physicalMemoryManager.unused);

    // Change it's type to typeinfo
    sliceinfo->type = L1PageTable;

    // Put it on the allocated stack
    push_to_ll(&physicalMemoryManager.allocated, sliceinfo);

    // Pre-zero the L1PageTable
    memset(&sliceinfo->slice->l1pt, 0, sizeof(struct L1PageTable));

    return &sliceinfo->slice->l1pt;
}

struct L2PageTable * pmm_allocate_l2_pagetable() {
    // First test if there's a partial allocated l2 pagetable
    if (physicalMemoryManager.l2ptPartialFree != NULL) {
        struct MemorySliceInfo * sliceinfo = physicalMemoryManager.l2ptPartialFree;

        uint32_t index = first_free(sliceinfo->filled);

        struct L2PageTable * newl2pt = &sliceinfo->slice->l2pt[index];

        sliceinfo->filled |= (1u << index);

        // If it is filled
        if (sliceinfo->filled == 0xff) {
            // Remove from the partial free list
            // We can ignore the return value here as we already got it.
            pop_from_ll(&physicalMemoryManager.l2ptPartialFree);

            // add to allocated list
            push_to_ll(&physicalMemoryManager.allocated, sliceinfo);
        }

        memset(newl2pt, 0, sizeof(struct L2PageTable));
        return newl2pt;
    } else {
        struct MemorySliceInfo * sliceinfo = pop_from_ll(&physicalMemoryManager.unused);

        // Change it's type to typeinfo
        sliceinfo->type = L2PageTable;
        sliceinfo->filled = 1;

        // Put it on the partially allocated list
        push_to_ll(&physicalMemoryManager.l2ptPartialFree, sliceinfo);

        struct L2PageTable * newl2pt = &sliceinfo->slice->l2pt[0];
        memset(newl2pt, 0, sizeof(struct L2PageTable));
        return newl2pt;
    }
}

struct Page * pmm_allocate_page() {
    // First test if there's a partial allocated page page
    if (physicalMemoryManager.pagePartialFree != NULL) {
        struct MemorySliceInfo * sliceinfo = physicalMemoryManager.pagePartialFree;

        uint32_t index = first_free(sliceinfo->filled);


        struct Page * newpage = &sliceinfo->slice->page[index];

        sliceinfo->filled |= (1u << index);

        // If it is filled (2 pages)
        if (sliceinfo->filled == 0b11) {
            // Remove from the partial free list
            // We can ignore the return value here as we already got it.
            pop_from_ll(&physicalMemoryManager.pagePartialFree);

            // add to allocated list
            push_to_ll(&physicalMemoryManager.allocated, sliceinfo);
        }

        memset(newpage, 0, sizeof(struct Page));
        return newpage;
    } else {
        struct MemorySliceInfo * sliceinfo = pop_from_ll(&physicalMemoryManager.unused);

        // Change it's type to typeinfo
        sliceinfo->type = Page;
        sliceinfo->filled = 0b0001;

        // Put it on the partially allocated list
        push_to_ll(&physicalMemoryManager.pagePartialFree, sliceinfo);

        struct Page * newpage = &sliceinfo->slice->page[0];
        memset(newpage, 0, sizeof(struct Page));
        return newpage;
    }
}

void pmm_free_page(struct Page * p) {
    // works cuz rounding (we think, might just work because random luck)
    struct MemorySliceInfo * info = pmm_get_sliceinfo_for_slice((union MemorySlice *)p);

    if (info->reserved) {
        WARN("Attempted to free reserved slice");
        return;
    }

    if (info->filled == 0b11) {
        remove_element_ll(&physicalMemoryManager.allocated, info);
    } else {
        remove_element_ll(&physicalMemoryManager.pagePartialFree, info);
    }

    // compute which subelement we are
    size_t offset_from_slice_start = ((size_t)p - (size_t)info->slice);
    size_t index_in_slice = offset_from_slice_start / sizeof(struct Page);

    // correctly clear the bit from filled
    info->filled &= ~(1u << index_in_slice);

    // if there was only one l2pt in this slice, put it on unallocated
    if (info->filled == 0x0) {
        push_to_ll(&physicalMemoryManager.unused, info);
    } else {
        push_to_ll(&physicalMemoryManager.pagePartialFree, info);
    }
}

void pmm_free_l2_pagetable(struct L2PageTable * pt) {
    // works cuz rounding (we think, might just work because random luck)
    struct MemorySliceInfo * info = pmm_get_sliceinfo_for_slice((union MemorySlice *)pt);

    if (info->reserved) {
        WARN("Attempted to free reserved slice");
        return;
    }

    if (info->filled == 0xff) {
        remove_element_ll(&physicalMemoryManager.allocated, info);
    } else {
        remove_element_ll(&physicalMemoryManager.l2ptPartialFree, info);
    }

    // compute which subelement we are
    size_t offset_from_slice_start = ((size_t)pt - (size_t)info->slice);
    size_t index_in_slice = offset_from_slice_start / sizeof(struct L2PageTable);

    // correctly clear the bit from filled
    info->filled &= ~(1u << index_in_slice);

    // if there was only one l2pt in this slice, put it on unallocated
    if (info->filled == 00) {
        push_to_ll(&physicalMemoryManager.unused, info);
    } else {
        push_to_ll(&physicalMemoryManager.l2ptPartialFree, info);
    }
}

void push_to_ll(struct MemorySliceInfo ** head, struct MemorySliceInfo * entry) {
    if (*head != NULL) { (*head)->prev = entry; }
    entry->next = *head;
    *head = entry;
}

struct MemorySliceInfo * pop_from_ll(struct MemorySliceInfo ** head) {
    struct MemorySliceInfo * top = *head;
    *head = (*head)->next;
    if (*head != NULL) { (*head)->prev = NULL; }

    top->next = NULL;
    top->prev = NULL;

    return top;
}

void remove_element_ll(struct MemorySliceInfo ** head, struct MemorySliceInfo * entry) {
    if (entry->next != NULL) { entry->next->prev = entry->prev; }

    if (entry->prev != NULL) {
        entry->prev->next = entry->next;
    } else {
        *head = entry->next;
    }

    entry->next = NULL;
    entry->prev = NULL;
}

size_t first_free(uint16_t filled) {
    filled = ~filled;
    // 32 because clz on arm counts from the 32nd aka most significant bit.
    return 32u - __builtin_clz(filled & (uint16_t)(-filled)) - 1u;
}
