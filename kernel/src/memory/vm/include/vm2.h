#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <constants.h>

/// A L1PagetableEntry is an entry in the top level pagetable.
/// There is only one L1 pagetable and it is always located at address 0x4000.
/// Relevant manual section: http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/DDI0301H_arm1176jzfs_r0p7_trm.pdf
/// * 6.1
/// * 6.5 (memory access control)
/// * 6.11.1 (entry layout)
/// * 6.13 (control registers)
/// * 6.10 (page faults and aborts)
typedef union L1PagetableEntry{
    uint32_t entry;
    struct {
        /// This 2 bit field gives what type of entry this is.
        /// 00 for invalid pages (pagefault)
        /// 01 for coarse pages (64kb).
        /// 10 for sections and supersections. (see bit 18)
        /// 11 for invalid pages (pagefault)
        /// Note that for supersections you have to repeat this entry 16x to fill up all 1mb entries that would lie within it,
        /// and the first of these 16 entries must be on a 16-word boundary
        uint32_t type: 2;

        /// Should be zero
        uint32_t sbz1: 1;

        /// Non-secure bit. This is used by the security extensions (TrustZone).
        uint32_t nonSecure: 1;

        /// Should be zero
        uint32_t sbz2: 1;

        /// Domain. This is used by the security extensions (TrustZone).
        uint32_t domain: 4;

        /// (P) ECC bit (some processors support this but the ARM1176 does *NOT*)
        uint32_t ECC: 1;

        /// Top 22 bits of the base address. Points to a L2 pagetable.
        /// formula:
        /// base_address = (address >> 10);
        uint32_t base_address: 22;

    } coarse;

    struct {
        /// This 2 bit field gives what type of entry this is.
        /// 00 for invalid pages (pagefault)
        /// 01 for coarse pages (64kb).
        /// 10 for sections and supersections. (see bit 18)
        /// 11 for invalid pages (pagefault)
        /// Note that for supersections you have to repeat this entry 16x to fill up all 1mb entries that would lie within it,
        /// and the first of these 16 entries must be on a 16-word boundary
        uint32_t type: 2;

        /// If set, this page is bufferable
        uint32_t bufferable: 1;

        /// If set, this page is cachable
        uint32_t cachable: 1;

        /// Should be zero
        uint32_t sbz2: 1;

        /// Domain. This is used by the security extensions (TrustZone).
        uint32_t domain: 4;

        /// (P) ECC bit (some processors support this but the ARM1176 does *NOT*)
        uint32_t ECC: 1;

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
        uint32_t accessPermissions: 2;

        /// Type Extension
        ///     TEX C B     Description                                         Memory type         Sharable
        ///     000 0 0     Strongly ordered                                    Strongly ordered    yes
        ///     000 0 1     Shared device                                       Device              yes
        ///     000 1 0     Outer and Inner Write-Through,No Allocate on Write  Normal              Page sharable if S bit is set
        ///     000 1 1     Outer and Inner Write-Back,No Allocate on Write     Normal              Page sharable if S bit is set
        ///     001 0 0     Outer and Inner Noncacheable                        Normal              Page sharable if S bit is set
        ///     001 0 1     Reserved
        ///     001 1 0     Reserved
        ///     001 1 1     Outer and Inner Write-Back, Allocate on Write       Normal              Page sharable if S bit is set
        ///     010 0 0     Non shared device                                   Device              no
        ///     010 0 1     Reserved
        ///     010 1 X     Reserved
        ///     011 X X     Reserved
        ///     1BB A A     Cached Memory                                       Normal              Page sharable if S bit is set
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
        /// [reference manual](http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/DDI0301H_arm1176jzfs_r0p7_trm.pdf)
        uint32_t TEX: 3;

        /// Access extended bit. See `accessPermissions`
        uint32_t accessExtended: 1;

        /// Marks the page sharable (see TEX). Also named S-bit
        uint32_t sharable:  1;

         /// Not Global (ng). Determines how this is marked in the TLB.
        uint32_t notglobal: 1;

        /// Supersection identifier is '1' if this entry is a supersection, is '0' if this is a normal section
        uint32_t supersection: 1;

        /// Non-secure bit. This is used by the security extensions (TrustZone).
        uint32_t nonSecure: 1;

        /// Top 12 bits of the base address. Pointer to the first byte in a 1mb-large block.
        /// If this entry refers to a supersection, the lower 4 bits Should Be Zero.
        /// The top 8 bits now refer to a 16mb-large block
        ///
        /// formulae:
        /// section_base_address = (address >> 20);
        /// supersection_base_address = (address >> 24);
        uint32_t base_address: 12;
    } section;
} L1PagetableEntry;

///
typedef union {
    uint32_t entry;
    struct {
        /// This 2 bit field gives what type of entry this is.
        /// 00 for invalid pages (pagefault)
        /// 01 for large pages (64kb).
        /// 10 for extended small pages (4kb)
        /// 11 for extended *non-executable* small pages. (4kb)
        /// Note that for large pages you have to repeat this entry 16x to fill up all 4kb entries that would lie within it,
        /// and the first of these 16 entries must be on a 16-word boundary
        uint32_t type: 2;

        /// If set, this page is bufferable
        uint32_t bufferable: 1;

        /// If set, this page is cachable
        uint32_t cachable: 1;


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
        uint32_t accessPermissions: 2;

        /// reserved (and Should Be Zero)
        uint32_t shouldBeZero1: 3;

        /// Access extended bit. See `accessPermissions`
        uint32_t accessExtended: 1;

        /// Marks the page sharable (see TEX). Also named S-bit
        uint32_t sharable: 1;

        /// Not Global (ng). Determines how this is marked in the TLB.
        uint32_t notglobal: 1;

        /// Type Extension
        /// For explanation: see [L1PagetableEntry.section.TEX]
        uint32_t TEX: 3;

        /// Mark this page as non executable.
        uint32_t nonExecutable: 1;

        /// Top 16 bits of the base address. Pointer to the first byte in a 64kb-large block.
        /// formula:
        /// base_address = (address >> 16);
        uint32_t base_address: 16;

    } __attribute__((packed)) largepage;

    struct {
        /// This 2 bit field gives what type of entry this is.
        /// 0 for invalid pages (pagefault)
        /// 1 for large pages (64kb).
        /// 2 for extended small pages (4kb)
        /// 3 for exetended *non-executable* small pages. (4kb)
        /// Note that for large pages you have to repeat this entry 16x to fill up all 4kb entries that would lie within it,
        /// and the first of these 16 entries must be on a 16-word boundary
        uint32_t type: 2;

        /// If set, this page is bufferable (B)
        uint32_t bufferable: 1;

        /// If set, this page is cachable (S)
        uint32_t cachable: 1;


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
        uint32_t accessPermissions: 2;

        /// Type Extension
        /// For explanation: see [L2PagetableEntry.largepage.TEX]
        uint32_t TEX: 3;

        /// Access extended bit. See `accessPermissions` (APX)
        uint32_t accessExtended: 1;

        /// Marks the page sharable (see TEX). Also named S-bit
        uint32_t sharable: 1;

        /// Not Global (ng). Determines how this is marked in the TLB.
        uint32_t notglobal: 1;

        /// Top 20 bits of the base address. Pointer to the first byte in a 4kb-large block.
        /// formula:
        /// base_address = (address >> 12);
        uint32_t base_address: 20;

    } __attribute__((packed)) smallpage;
} L2PagetableEntry;

/// The representation of an L1Pagetable
struct L1PageTable {
    L1PagetableEntry entries[0x1000];
};

/// The representation of an L1Pagetable
struct L2PageTable {
    L2PagetableEntry entries[0x100];
};

/// Should be called early, initiliazes everything vm2 needs
void vm2_prepare();
/// Actually enables the MMU and switches the kernel to higher half
void vm2_start();
void vm2_map_virtual_to_physical_l1(size_t virtual, size_t physical);
void vm2_map_nmegabytes_1to1(size_t address, size_t n);

/// From the `kernel.ld` linker file. These are not arrays but this is how you refer to the pointers by the linker script.
extern const size_t KERNEL_BASE[]; // 0x8000
extern const size_t KERNEL_TOP[];
extern const size_t STACK_TOP[];

/// Above 3Gigs is the virtual kernel area, this also includes all Virtual Memory constructs, and the kernel stack.
#define KERNEL_VIRTUAL_OFFSET   (3u * Gibibyte)

/// Make the kernel start in virtual memory at 3GB
#define KERNEL_VIRTUAL_START    (KERNEL_VIRTUAL_OFFSET + (size_t)KERNEL_BASE)
#define KERNEL_VIRTUAL_END      (KERNEL_VIRTUAL_OFFSET + (size_t)KERNEL_TOP)

/// The L1 pagetable starts at 0x4000 and is itself 0x4000 bytes long. (0x1000 entries of 4 bytes).
/// The kernel starts at 0x8000 which is exactly at the end of the pagetable. (What a coincidence *gasp*)
#define PhysicalL1PagetableLocation     0x4000
#define VirtualL1PagetableLocation      (KERNEL_VIRTUAL_OFFSET + PhysicalL1PagetableLocation)

// The end of the kernel is guaranteed 1MiB aligned due to our linking script
#define VirtualL2PagetableLocation      KERNEL_VIRTUAL_END
#define PhysicalL2PagetableLocation     ((size_t)KERNEL_TOP)

#endif
