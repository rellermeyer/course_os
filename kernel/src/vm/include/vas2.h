#ifndef VAS_2_H
#define VAS_2_H

#include <asid_allocator.h>
#include <vm2.h>
#include <vp_array_list.h>

#define VAS2_INITIAL_PAGE_LIST_CAPACITY 10

struct vas2 {
    struct ASIDDescriptor tlbDescriptor;
    struct L1PageTable * l1PageTable;
    VPArrayList * l2tables;
    VPArrayList * pages;
};


/// Creates a new virtual address spce for a process.
struct vas2 * create_vas();

/// Used in context switching. Sets up the mmu for a process to run.
void switch_to_vas(struct vas2 * vas);

/// Freeing a vas clears all pagetables and pages associated with it.
/// It essentially frees all the memory a process has.
void free_vas(struct vas2 * vas);

/// Creates a new page starting at the first page boundary below address.
/// This page is thus the page around the address. The address does not have to be
/// page boundary aligned. The new page is added to the vas and made executable if requested.
void allocate_page(struct vas2 * vas, size_t address, bool executable);

#endif
