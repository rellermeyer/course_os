#ifndef VM_H
#define VM_H

#include <constants.h>
#include <stdbool.h>
#include <stdint.h>


/// Permissions for mapping a page
enum Access {
    KernelRO,  // kernel ro, user no access
    KernelRW,  // kernel rw, user no access

    // Everything below this is accessible for the kernel and the user.

    UserRO,  // kernel rw, user ro
    UserRW,  // kernel rw, user rw
};

struct PagePermission {
    enum Access access;  // Access permissions for this page as specified above
    bool executable;     // If the Page should be executable or not
};

/// A L1PagetableEntry is an entry in the top level pagetable.
/// Relevant manual section:
/// http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/DDI0301H_arm1176jzfs_r0p7_trm.pdf
/// * 6.1
/// * 6.5 (memory access control)
/// * 6.11.1 (entry layout)
/// * 6.13 (control registers)
/// * 6.10 (page faults and aborts)
typedef union L1PagetableEntry {
    uint32_t entry;
    /// A coarse l1pt entry means an l1pt entry that points to an l2pt.
    /// The other possibility is a (super)section, where the l1pt maps a
    /// far larger area of memory at once, without requiring an l2pt.
    struct {
        /// This 2 bit field gives what type of entry this is.
        /// 00 for invalid pages (pagefault)
        /// 01 for coarse pages (64kb).
        /// 10 for sections and supersections. (see bit 18)
        /// 11 for invalid pages (pagefault)
        /// Note that for supersections you have to repeat this entry 16x to fill up all 1mb entries
        /// that would lie within it, and the first of these 16 entries must be on a 16-word
        /// boundary
        uint32_t type : 2;

        /// Should be zero
        uint32_t sbz1 : 1;

        /// Non-secure bit. This is used by the security extensions (TrustZone).
        uint32_t nonSecure : 1;

        /// Should be zero
        uint32_t sbz2 : 1;

        /// Domain. This is used by the security extensions (TrustZone).
        uint32_t domain : 4;

        /// (P) ECC bit (some processors support this but the ARM1176 does *NOT*)
        uint32_t ECC : 1;

        /// Top 22 bits of the base address. Points to a L2 pagetable.
        /// formula:
        /// base_address = (address >> 10);
        uint32_t base_address : 22;

    } coarse;

    struct {
        /// This 2 bit field gives what type of entry this is.
        /// 00 for invalid pages (pagefault)
        /// 01 for coarse pages (64kb).
        /// 10 for sections and supersections. (see bit 18)
        /// 11 for invalid pages (pagefault)
        /// Note that for supersections you have to repeat this entry 16x to fill up all 1mb entries
        /// that would lie within it, and the first of these 16 entries must be on a 16-word
        /// boundary
        uint32_t type : 2;

        /// If set, this page is bufferable
        uint32_t bufferable : 1;

        /// If set, this page is cachable
        uint32_t cachable : 1;

        /// Should be zero
        uint32_t sbz2 : 1;

        /// Domain. This is used by the security extensions (TrustZone).
        uint32_t domain : 4;

        /// (P) ECC bit (some processors support this but the ARM1176 does *NOT*)
        uint32_t ECC : 1;

        /// Access permissions
        /// accessExtended = 0:
        ///             Kernel:         User:
        ///     00      No access       No access   (Recommended)
        ///     01      Read/Write      No access
        ///     10      Read/Write      Read only
        ///     11      Read/Write      Read/Write
        /// accessExtended = 1:
        ///     00      Reserved        Reserved
        ///     01      Read only       No access
        ///     10      Read only       Read only
        ///     11      Read only       Read only
        uint32_t accessPermissions : 2;

        /// Type Extension
        ///     TEX C B     Description                                         Memory type Sharable
        ///     000 0 0     Strongly ordered                                    Strongly ordered yes
        ///     000 0 1     Shared device                                       Device yes 000 1 0
        ///     Outer and Inner Write-Through,No Allocate on Write  Normal              Page
        ///     sharable if S bit is set 000 1 1     Outer and Inner Write-Back,No Allocate on Write
        ///     Normal              Page sharable if S bit is set 001 0 0     Outer and Inner
        ///     Noncacheable                        Normal              Page sharable if S bit is
        ///     set 001 0 1     Reserved 001 1 0     Reserved 001 1 1     Outer and Inner
        ///     Write-Back, Allocate on Write       Normal              Page sharable if S bit is
        ///     set 010 0 0     Non shared device                                   Device no 010 0
        ///     1     Reserved 010 1 X     Reserved 011 X X     Reserved 1BB A A     Cached Memory
        ///     Normal              Page sharable if S bit is set
        ///                 BB = Outer policy
        ///                 AA = Inner policy
        ///
        ///     BB *or* AA bits:
        ///     00      Non-cachable
        ///     01      Write-back cached, write allocate
        ///     10      Write-through cached, no allocate on write
        ///     11      Write-back cached, no allocate on write
        ///
        /// For more info (and there is a lot more!), see table 6-4 in the ARM1176jzf-s
        /// [reference
        /// manual](http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/DDI0301H_arm1176jzfs_r0p7_trm.pdf)
        uint32_t TEX : 3;

        /// Access extended bit. See `accessPermissions`
        uint32_t accessExtended : 1;

        /// Marks the page sharable (see TEX). Also named S-bit
        uint32_t sharable : 1;

        /// Not Global (ng). Determines how this is marked in the TLB.
        uint32_t notglobal : 1;

        /// Supersection identifier is '1' if this entry is a supersection, is '0' if this is a
        /// normal section
        uint32_t supersection : 1;

        /// Non-secure bit. This is used by the security extensions (TrustZone).
        uint32_t nonSecure : 1;

        /// Top 12 bits of the base address. Pointer to the first byte in a 1mb-large block.
        /// If this entry refers to a supersection, the lower 4 bits Should Be Zero.
        /// The top 8 bits now refer to a 16mb-large block
        ///
        /// formulae:
        /// section_base_address = (address >> 20);
        /// supersection_base_address = (address >> 24);
        uint32_t base_address : 12;
    } section;
} L1PagetableEntry;

/// An L2PagetableEntry is an entry inside the L2Pagetable.
/// A single entry can address 4KiB of memory.
/// You shouldn't need to make any entries manually as you can use the [vm2_allocate_page] function
/// to allocate space for you.
typedef union L2PagetableEntry {
    uint32_t entry;
    struct {
        /// This 2 bit field gives what type of entry this is.
        /// 00 for invalid pages (pagefault)
        /// 01 for large pages (64kb).
        /// 10 for extended small pages (4kb)
        /// 11 for extended *non-executable* small pages. (4kb)
        /// Note that for large pages you have to repeat this entry 16x to fill up all 4kb entries
        /// that would lie within it, and the first of these 16 entries must be on a 16-word
        /// boundary
        uint32_t type : 2;

        /// If set, this page is bufferable
        uint32_t bufferable : 1;

        /// If set, this page is cachable
        uint32_t cachable : 1;


        /// Access permissions
        /// accessExtended = 0:
        ///             Kernel:         User:
        ///     00      No access       No access   (Recommended)
        ///     01      Read/Write      No access
        ///     10      Read/Write      Read only
        ///     11      Read/Write      Read/Write
        /// accessExtended = 1:
        ///     00      Reserved        Reserved
        ///     01      Read only       No access
        ///     10      Read only       Read only
        ///     11      Read only       Read only
        uint32_t accessPermissions : 2;

        /// reserved (and Should Be Zero)
        uint32_t shouldBeZero1 : 3;

        /// Access extended bit. See `accessPermissions`
        uint32_t accessExtended : 1;

        /// Marks the page sharable (see TEX). Also named S-bit
        uint32_t sharable : 1;

        /// Not Global (ng). Determines how this is marked in the TLB.
        uint32_t notglobal : 1;

        /// Type Extension
        /// For explanation: see [L1PagetableEntry.section.TEX]
        uint32_t TEX : 3;

        /// Mark this page as non executable.
        uint32_t nonExecutable : 1;

        /// Top 16 bits of the base address. Pointer to the first byte in a 64kb-large block.
        /// formula:
        /// base_address = (address >> 16);
        uint32_t base_address : 16;
    } __attribute__((packed)) largepage;

    struct {
        /// This 2 bit field gives what type of entry this is.
        /// 0 for invalid pages (pagefault)
        /// 1 for large pages (64kb).
        /// 2 for extended small pages (4kb)
        /// 3 for extended *non-executable* small pages. (4kb)
        /// Note that for large pages you have to repeat this entry 16x to fill up all 4kb entries
        /// that would lie within it, and the first of these 16 entries must be on a 16-word
        /// boundary
        uint32_t type : 2;

        /// If set, this page is bufferable (B)
        uint32_t bufferable : 1;

        /// If set, this page is cachable (S)
        uint32_t cachable : 1;


        /// Access permissions
        /// accessExtended = 0:
        ///             Kernel:         User:
        ///     00      No access       No access   (Recommended)
        ///     01      Read/Write      No access
        ///     10      Read/Write      Read only
        ///     11      Read/Write      Read/Write
        /// accessExtended = 1:
        ///     00      Reserved        Reserved
        ///     01      Read only       No access
        ///     10      Read only       Read only
        ///     11      Read only       Read only
        uint32_t accessPermissions : 2;

        /// Type Extension
        /// For explanation: see [L2PagetableEntry.largepage.TEX]
        uint32_t TEX : 3;

        /// Access extended bit. See `accessPermissions` (APX)
        uint32_t accessExtended : 1;

        /// Marks the page sharable (see TEX). Also named S-bit
        uint32_t sharable : 1;

        /// Not Global (ng). Determines how this is marked in the TLB.
        uint32_t notglobal : 1;

        /// Top 20 bits of the base address. Pointer to the first byte in a 4kb-large block.
        /// formula:
        /// base_address = (address >> 12);
        uint32_t base_address : 20;

    } __attribute__((packed)) smallpage;
} L2PagetableEntry;

/// The representation of an L1Pagetable
struct L1PageTable {
    L1PagetableEntry entries[0x800];
};

/// The representation of an L1Pagetable
struct L2PageTable {
    L2PagetableEntry entries[0x100];
};

/// Should be called early, initializes everything vm2 needs
void vm2_start();

/// Takes an L1PageTableEntry containing a physical address (in .*.base_address) and maps it to a
/// physical addres. The `bool remap` can be set to true and then this will overwrite whatever was
/// previously mapped at this virtual address in the pagetable. Will return if it did replace
/// something. Generally remap should be set to false, and the kernel should panic when this happen.
/// It usually indicates something went terribly enormously incredibly really really *really* wrong.
bool vm2_l1_map_physical_to_virtual(struct L1PageTable * pt,
                                    union L1PagetableEntry entry,
                                    size_t virtual,
                                    bool remap);

/// Peripheral mappings
/// Request a section of n megabytes virtual memory which is mapped to a physical section of ram of
/// the same n megabytes in which some mmio is located. Returns the virtual address.
size_t vm2_map_peripheral(size_t physical, size_t n_mebibytes);

/// Maps a new 4KiB page at a virtual address. Returns a reference to this
/// page or NULL if unsuccessful. The physical location of this page is determined by the
/// [PMM](pmm.c). Since this allocates a 4KiB page, it has to go through L2Pagetables. It will
/// create the right L2 pagetables as it needs. You can make the allocated page executable with the
/// second to last parameter. If you want to keep track of your allocated L2Pagetables you can pass
/// a double pointer to `created_l2pt` And the function will return the pointer to the allocated
/// L2Pagetable in there (or NULL if no allocation happened).
void * vm2_allocate_page(struct L1PageTable * l1pt,
                         size_t virtual,
                         bool remap,
                         struct PagePermission perms,
                         struct L2PageTable ** created_l2pt);

/// Frees a 4KiB page at a virtual address. The address does not need to be aligned. If the address
/// is not aligned, the aligned 4KiB page the address lies in is freed.
/// TODO: Does not yet free l2 pagetables when they become empty after enough pages are freed. For
/// TODO: processes this is never necessary as their pagetables are freed whenever the process
/// stops. For
/// TODO: the kernel it might be necessary.
/// Automatically unmaps the page from the l1pt it was in.
void vm2_free_page(struct L1PageTable * l1pt, size_t virtual);

/// Should be called after updating a pagetable.
void vm2_flush_caches();

/// Flushes the caches associated with an ASID.
void vm2_flush_caches_of_ASID(uint8_t id);

/// Enables a given l1 pagetable on the MMU.
void vm2_set_user_pagetable(struct L1PageTable * l1);

/// The kernel's L1 Pagetable.
extern struct L1PageTable * kernell1PageTable;

/// From the `kernel.ld` linker file. These are not arrays but this is how you refer to the pointers
/// by the linker script.
extern const size_t __KERNEL_BASE[];
extern const size_t __KERNEL_TOP[];
extern const size_t __KERNEL_VIRTUAL_OFFSET[];

/// Above 2Gigs is the virtual kernel area, this also includes all Virtual Memory constructs, and
/// the kernel stack.
#define KERNEL_VIRTUAL_OFFSET ((size_t)__KERNEL_VIRTUAL_OFFSET)

#define PHYS2VIRT(address) ((size_t)(address) + KERNEL_VIRTUAL_OFFSET)
#define VIRT2PHYS(address) ((size_t)(address)-KERNEL_VIRTUAL_OFFSET)

/// Make the kernel start in virtual memory at 2GB
#define KERNEL_VIRTUAL_START ((size_t)__KERNEL_BASE)
#define KERNEL_VIRTUAL_END   ((size_t)__KERNEL_TOP)

/// Location of the Kernel's Physical Memory Manager's Info structs. Can grow to a max of 16MiB when
/// using 4GiB RAM.
#define KERNEL_PMM_BASE KERNEL_VIRTUAL_END

// Location of the vector table in memory table.
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/BABIFJFG.html
#define HIGH_VECTOR_LOCATION 0xFFFF0000

/// From this address down, mmio devices are mapped in the kernel's virtual address space.
#define KERNEL_MMIO_BASE ((4 * Gibibyte) - (1 * Mebibyte))

/// Address space for the kernel heap, grows towards the mmio
#define KERNEL_HEAP_BASE (3 * Gibibyte)

#define KERNEL_PHYSICAL_START (KERNEL_VIRTUAL_START - KERNEL_VIRTUAL_OFFSET)
#define KERNEL_PHYSICAL_END   (KERNEL_VIRTUAL_END - KERNEL_VIRTUAL_OFFSET)

/// The L1 pagetable starts at 0x4000 and is itself 0x4000 bytes long. (0x1000 entries of 4 bytes).
/// The kernel starts at 0x8000 which is exactly at the end of the pagetable. (What a coincidence
/// *gasp*)
#define PhysicalL1PagetableLocation 0x4000
#define VirtualL1PagetableLocation  (KERNEL_VIRTUAL_OFFSET + PhysicalL1PagetableLocation)

#define PAGE_SIZE (4 * Kibibyte)

/*
 * general purpose useful macros
 */

/// aligns an address to the *next* boundary of size n.
/// only works where n is a power of 2
#define ALIGN(address, n) ((((size_t)(address) + (size_t)(n)-1) & ~((size_t)(n)-1)));

/// The purpose of the Data Synchronization Barrier operation is to ensure that
/// all outstanding explicit memory transactions complete before any following instructions begin.
/// This ensures that data in memory is up to date before the processor executes any more
/// instructions.
#define DATA_SYNC_BARRIER() asm volatile("eor r0, r0, r0\nmcr p15, 0, r0, c7, c10, 4");

#endif
