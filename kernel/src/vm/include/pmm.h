/// This page allocator was completely designed and implemented by
/// Jonathan Donszelmann    <jonabent@gmail.com>
/// Victor Roest            <victor@xirion.next>
///
/// ## Description
///
/// It can allocate pages of sizes:
///     * 8KiB (L1 pagetables)
///     * 1KiB  (L2 pagetables)
///     * 4KiB  (Normal data pages)
/// All pages are aligned to their own size (i.e. The 8KiB pages are 8K algined)
///
/// It stores all datastructures it uses in the same pages it allocates,
/// and can therefore be used without any existing allocator allocating it's space.
///
/// This allocator has been made specifically for 32 bit ARM cpus.
/// This means it can *not* be used on x86 without adaptation.
/// It relies on addresses being 32 bits long and being aligned to boundaries to save (a lot) of
/// space.
///
/// ## Overhead
/// Since this allocator also has to store some datastructures in itself, it obviously has some
/// overhead. This overhead is 16kb for every 512 pages of size 16kb allocated. For 64 bit systems
/// this ovehead would be larger.
///
/// ## Time complexity
/// All operations performed on this allocator are O(1). They are in no way dependent on the size of
/// memory. With the exception of one method: The initialization of the allocator will loop once
/// through all of physical memory in increments of 16kb.
///
/// ## Definitions
///
/// In this code and the comments, we refer to the following things:
/// * (memory)slice A 16kb area of physical ram
/// * page          A 4 kb area of physical ram (used for virtual user memory).
/// * sliceinfo     A 12 byte element in the allocators datastructures, describing a single 16kb
/// block
/// * bucket        A piece of memory that can be referred to by a single 16kb page full of
/// sliceinfo structs.
///                 The size of a bucket is therefore 16kb * 682b which is just over 10.5 megabytes
/// * bucketinfo    A slice containing an array of sliceinfo structs describing the contents of a
/// bucket.

#ifndef PMM_H
#define PMM_H

#include <stdbool.h>
#include <stdint.h>

#include "vm2.h"


/// The type of an 8KiB Slice
enum MemoryType {
    L1PageTable,
    L2PageTable,
    Page,
    BucketInfo,
};

/// A page with 4KiB of data.
struct Page {
    uint8_t data[4096];
};

// Early definition so we can have the cyclic dependency between MemorySliceInfo and MemorySlice.
union MemorySlice;

/// A MemorySliceInfo contains all information for describing an 8KiB Slice of memory.
/// These are allocated inside so-called BucketInfo Slices.
struct MemorySliceInfo {
    union {
        uint32_t entry;
        struct {
            /// The type of the memory this SliceInfo is describing.
            enum MemoryType type : 2;

            /// To determine how full this slice is if it describes a list of structures of less
            /// than 8KiB. This is a BitVector where 1 means allocated and 0 means free.
            uint32_t filled : 8;

            /// Unused bits
            uint32_t unused : 22;
        };
    };

    /// Pointers used to build a linked list of SliceInfos
    struct MemorySliceInfo * next;
    struct MemorySliceInfo * prev;

    /// Pointer to the actual memory this SliceInfo is describing.
    union MemorySlice * slice;
};


#define SLICEINFO_PER_SLICE 512  // sizeof(union MemorySlice) / sizeof(struct MemorySliceInfo)
#define L2TABLES_PER_SLICE  8    // sizeof(union MemorySlice) / sizeof(struct L2PageTable)
#define PAGES_PER_SLICE     2    // sizeof(union MemorySlice) / sizeof(struct Page)

/// A Memory slice of 8KB, that's also 8kb aligned.
/// A slice of 8kb can fit one of three things:
///     * 2   physical pages of 4kb
///     * 1   l1 pagetable of 8kb
///     * 8   l2 pagetabke of 1kb
///     * 512 MemorySliceInfo structs indexing other pages
union MemorySlice {
    struct L1PageTable l1pt;
    struct Page page[PAGES_PER_SLICE];
    struct L2PageTable l2pt[L2TABLES_PER_SLICE];
    // The sliceinfo array (bucketinfo) that contains the information of all Memoryslices in the
    // next bucket.
    struct MemorySliceInfo bucketinfo[SLICEINFO_PER_SLICE];
};

/// The main data struct for the PhysicalMemoryManager (PMM). This struct gets created when calling
/// [pmm_init].
struct PhysicalMemoryManager {
    /// Two linked lists of unused and allocated slices, referred to by their SliceInfos.
    struct MemorySliceInfo * unused;
    struct MemorySliceInfo * allocated;

    /// The partially allocated entries, these can be linked lists or NULL.
    struct MemorySliceInfo * l2ptPartialFree;
    struct MemorySliceInfo * pagePartialFree;

    /// The start address of the region we are allowed to allocate in.
    size_t start;
    /// The end address of the region we are allowed to allocate in.
    size_t end;
};

__attribute__((__common__))  struct PhysicalMemoryManager physicalMemoryManager;

// Allocator specific operations

/**
 * Initializes the PMM over the specified memory region.
 *
 * This functions will also set the global [physicalMemoryManager] variable.
 *
 * @param start of the memory region to be managed by the PMM
 * @param end of the memory region to be managed by the pmm
 */
void pmm_init(size_t start, size_t end);

/**
 * Allocates a new MemorySliceInfo inside the PMM.
 * For internal use only.
 * @return a pointer to the allocated slice.
 */
struct MemorySliceInfo * pmm_new_sliceinfo_slice();

/**
 * This function uses pointer arithmetic to determine the associated [MemorySliceInfo] for a
 * [MemorySlice]. Despite the fact that we store the allocated pages in a linked list this function
 * is O(1) due to aforementioned, pointer arithmetic.
 * @param slice The slice to get the sliceinfo for.
 * @return The [MemorySliceInfo] pointer for the given [MemorySlice].
 */
struct MemorySliceInfo * pmm_get_sliceinfo_for_slice(union MemorySlice * slice);


/**
 * Allocates an L1Pagetable.
 * @return A pointer to said pagetable.
 */
struct L1PageTable * pmm_allocate_l1_pagetable();

/**
 * Frees an L1Pagetable.
 * @param pt The [struct L1PageTable] to free.
 */
void pmm_free_l1_pagetable(struct L1PageTable * pt);

/**
 * Allocates an L1Pagetable.
 * @return A pointer to said pagetable.
 */
struct L2PageTable * pmm_allocate_l2_pagetable();

/**
 * Frees an L2Pagetable.
 * @param pt The [struct L2PageTable] to free.
 */
void pmm_free_l2_pagetable(struct L2PageTable * pt);

/**
 * Allocates a single 4KiB page.
 * This is the function you want to use if you just want an area of memory.
 * @return a pointer to the allocated [struct Page]
 */
struct Page * pmm_allocate_page();

/**
 * Frees a single 4KiB Page.
 * @param  p The [struct Page] to free.
 */
void pmm_free_page(struct Page * p);

#endif
