/// This code helped generate the data section of startup.s by printing out an entire l1pt
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define pt_size 0x1000

uint32_t pt[pt_size] = {0};

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80000000 ? '1' : '0'), \
  (byte & 0x40000000 ? '1' : '0'), \
  (byte & 0x20000000 ? '1' : '0'), \
  (byte & 0x10000000 ? '1' : '0'), \
  (byte & 0x08000000 ? '1' : '0'), \
  (byte & 0x04000000 ? '1' : '0'), \
  (byte & 0x02000000 ? '1' : '0'), \
  (byte & 0x01000000 ? '1' : '0'), \
  (byte & 0x00800000 ? '1' : '0'), \
  (byte & 0x00400000 ? '1' : '0'), \
  (byte & 0x00200000 ? '1' : '0'), \
  (byte & 0x00100000 ? '1' : '0'), \
  (byte & 0x00080000 ? '1' : '0'), \
  (byte & 0x00040000 ? '1' : '0'), \
  (byte & 0x00020000 ? '1' : '0'), \
  (byte & 0x00010000 ? '1' : '0'), \
  (byte & 0x00008000 ? '1' : '0'), \
  (byte & 0x00004000 ? '1' : '0'), \
  (byte & 0x00002000 ? '1' : '0'), \
  (byte & 0x00001000 ? '1' : '0'), \
  (byte & 0x00000800 ? '1' : '0'), \
  (byte & 0x00000400 ? '1' : '0'), \
  (byte & 0x00000200 ? '1' : '0'), \
  (byte & 0x00000100 ? '1' : '0'), \
  (byte & 0x00000080 ? '1' : '0'), \
  (byte & 0x00000040 ? '1' : '0'), \
  (byte & 0x00000020 ? '1' : '0'), \
  (byte & 0x00000010 ? '1' : '0'), \
  (byte & 0x00000008 ? '1' : '0'), \
  (byte & 0x00000004 ? '1' : '0'), \
  (byte & 0x00000002 ? '1' : '0'), \
  (byte & 0x00000001 ? '1' : '0')



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


static inline uint32_t address_to_index(uint32_t address) {
    return address >> 20u;
}

void map_virtual_to_physical(uint32_t virtual, L1PagetableEntry entry) {


    if (pt[address_to_index(virtual)] != 0){
        // The entry is already mapped
        printf("Request for already mapped address denied");
        exit(-1);
    }


    pt[address_to_index(virtual)] = entry.entry;
}

int main() {
    const int virtual_offset = 0xC0000000;

    uint32_t address = 0x00000000;

    // identitymap the first page
    map_virtual_to_physical(address, (L1PagetableEntry) {
            .section.type=2,
            .section.accessPermissions = 1, //rw kernel
            .section.base_address = address_to_index(address)
    });


    map_virtual_to_physical(address + virtual_offset, (L1PagetableEntry) {
            .section.type=2,
            .section.accessPermissions = 1, //rw kernel
            .section.base_address = address_to_index(address)
    });


    int numzeros = 0;
    for (uint32_t i = 0; i < pt_size; i++) {
        if (pt[i] == 0) {
            numzeros += 4;
        } else {
            if (numzeros != 0) {
                printf(".skip 0x%x\n", numzeros);
            }
            numzeros = 0;
            printf(".long 0b"BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(pt[i]));
        }
    }

    if (numzeros > 0) {
        printf(".skip 0x%x\n", numzeros);
    }
}

