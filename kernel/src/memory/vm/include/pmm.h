/// This page allocator was completely designed and implemented by
/// Jonathan Donszelmann    <jonabent@gmail.com>
/// Victor Roest            <victor@xirion.next>
///
/// ## Description
///
/// It can allocate pages of sizes:
///     * 16kb (L1 pagetables)
///     * 1kb  (L2 pagetables)
///     * 4kb  (Normal data pages)
/// All pages are aligned to their own size (i.e. The 16kb pages are 16k algined)
///
/// It stores all datastructures it uses in the same pages it allocates,
/// and can therefore be used without any existing allocator allocating it's space.
///
/// This allocator has been made specifically for 32 bit ARM cpus.
/// This means it can *not* be used on x86 without adaptation.
/// It relies on addresses being 32 bits long and being aligned to boundaries to save (a lot) of space.
///
/// ## Overhead
/// Since this allocator also has to store some datastructures in itself, it obviously has some overhead.
/// This overhead is 16kb for every 682 pages of size 16kb allocated. For 64 bit systems this ovehead would be larger.
///
/// ## Time complexity
/// All operations performed on this allocator are O(1). They are in no way dependent on the size of memory.
/// With the exception of one method: The initialization of the allocator will loop once through all of physical memory
/// in increments of 16kb.
///
/// ## Definitions
///
/// In this code and the comments, we refer to the following things:
/// * (memory)slice A 16kb area of physical ram
/// * page          A 4 kb area of physical ram (used for virtual user memory).
/// * sliceinfo     A 12 byte element in the allocators datastructures, describing a single 16kb block
/// * bucket        A piece of memory that can be referred to by a single 16kb page full of sliceinfo structs.
///                 The size of a bucket is therefore 16kb * 682b which is just over 10.5 megabytes
/// * bucketinfo    A slice containing an array of sliceinfo structs describing the contents of a bucket.
///
/// The 1024 byte constant you might see popping up refers to the maximum number of 16 byte sliceinfo structs
/// can fit in a single 16kb slice.

// TODO: Excluded regions. (for mmio for example)
// TODO: detect memory size.
#define PMM_TOP 0x10000000 // 256 MiB and less than all Peripheral bases

#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stdbool.h>
#include "vm2.h"


#define SLICEINFO_PER_SLICE 1024

/// The type of a 16kib Slice
enum MemoryType {
    L1PageTable,
    L2PageTable,
    Page,
    BucketInfo,
};

struct Page {
    uint8_t data[4096];
};


union MemorySlice;

struct MemorySliceInfo {
    union {
        uint32_t entry;
        struct {
            enum MemoryType type: 2;
            uint32_t filled: 16;
            /// Filler bits
            uint32_t unused: 14;
        };
    };

    struct MemorySliceInfo * next;
    struct MemorySliceInfo * prev;

    union MemorySlice * slice;
};

/// A Memory slice of 16KB, that's also 16kb aligned.
/// A slice of 16kb can fit one of three things:
///     * 4   physical pages of 4kb
///     * 1   l1 pagetable of 16kb
///     * 16  l2 pagetabke of 1kb
///     * 682 (rounded down) MemorySliceInfo structs indexing other pages
union MemorySlice {
    struct L1PageTable l1pt;
    struct Page page[4];
    struct L2PageTable l2pt[16];
    // The sliceinfo array (bucketinfo) that contains the information of all Memoryslices in the next bucket.
    struct MemorySliceInfo bucketinfo[SLICEINFO_PER_SLICE];
};


struct PhysicalMemoryManager {
    /// Two linked lists of unused and allocated slices, referred to by their SliceInfos.
    struct MemorySliceInfo * unused;
    struct MemorySliceInfo * allocated;

    /// The partially allocated entries, these either exist or don't (NULL).
    struct MemorySliceInfo * l2ptPartialFree;
    struct MemorySliceInfo * pagePartialFree;

    /// The start address of the region we are allowed to allocate in.
    size_t start;
    /// The end address of the region we are allowed to allocate in.
    size_t end;
};

struct PhysicalMemoryManager physicalMemoryManager;

// Allocator specific operations
void pmm_init(size_t start, size_t end);
struct MemorySliceInfo * pmm_new_sliceinfo_slice();
struct MemorySliceInfo * pmm_get_sliceinfo_for_slice(union MemorySlice * slice);


// Element operations
struct L1PageTable * pmm_allocate_l1_pagetable();
void pmm_free_l1_pagetable(struct L1PageTable * pt);
struct L2PageTable * pmm_allocate_l2_pagetable();
void pmm_free_l2_pagetable(struct L2PageTable * pt);
struct Page * pmm_allocate_page();
void pmm_free_page(struct Page * p);



#endif
