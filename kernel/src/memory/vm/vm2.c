#include <vm2.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <constants.h>
#include <pagealloc2.h>
#include <memory.h>

struct L1PageTable * kernell1PageTable = (struct L1PageTable *) PhysicalL1PagetableLocation;
struct L1PageTable * virtualkernell1PageTable = (struct L1PageTable *) VirtualL1PagetableLocation;
bool mmu_started = false;

static inline size_t l1pt_index(size_t address) {
    return address >> 20u;
}

void vm2_map_virtual_to_physical_l1(size_t virtual, size_t physical) {
    if(mmu_started) {
        // Outside users are not allowed to add entries to the pagetable
        // after the mmu has been started. They can request a new frame with
        // 
        panic();
    }

    if (kernell1PageTable->entries[l1pt_index(virtual)].entry != 0){
        // The entry is already mapped
        kprintf("Request for already mapped address denied");
        panic();

    }

    L1PagetableEntry entry = {
            .section.type = 2, // section
            .section.bufferable = 0,
            .section.cachable = 0,
            .section.accessPermissions = 0b01, // Kernel r/w, user no access.
            .section.TEX = 0, // Strongly ordered
            .section.accessExtended = 0, // r/w access
            .section.supersection = 0,
            .section.nonSecure = 1,
            .section.base_address = l1pt_index(physical),
    };

    kprintf("Mapping 1mb at virtual: 0x%x to physical: 0x%x\n", l1pt_index(virtual) << 20u, l1pt_index(physical)<<20u);

    kernell1PageTable->entries[l1pt_index(virtual)] = entry;
}

void vm2_map_nmegabytes_1to1(size_t address, size_t n) {
    if(n == 0) {
        panic();
    }

    // Iterate over the address in 1 MiB chunks.
    for (size_t newaddress = address; newaddress < (address + Mibibyte * n) ; newaddress += Mibibyte) {
        vm2_map_virtual_to_physical_l1(newaddress, newaddress);
    }
}

// Prepares the pagetable and add
void vm2_prepare() {
    // Clear the pagetable
    memset(kernell1PageTable, 0, sizeof(struct L1PageTable));
}




// Starts the actual MMU after this function we live in Virtual Memory
void vm2_start() {
    // Temporary, TODO: new range
    pagealloc_init(Gibibyte - 25 * Mibibyte, Gibibyte - 5 * Mibibyte);

    /// Identitymap the kernel and the stack. The kernel starts at 0x8000 but we already
    /// start mapping at 0x0000 as this is the first address before 0x8000 that's megabyte
    /// aligned. This has as a nice bonus that we also identitymap the L1 pagetable
    /// which lives at L1PagetableLocation=0x4000, and the interrupt vectors at 0x0.
    /// FIXME: We want to _eventually_ switch to a fully higher half kernel
    for (size_t i = 0; i < (size_t)KERNEL_TOP; i += Mibibyte) {
        vm2_map_nmegabytes_1to1(i, 1);
    }

    /// Map the kernel and the stack a second time to high virtual memory. Again this also
    /// maps the l1 pagetable here and the interrupt vectors at 0x0.
    for (size_t i = 0; i < (size_t)KERNEL_TOP; i += Mibibyte) {
        vm2_map_virtual_to_physical_l1(KERNEL_VIRTUAL_START + i, i);
    }

    /// Map 1 megabyte for the kernel L2 pagetables.
    vm2_map_virtual_to_physical_l1(VirtualL2PagetableLocation, PhysicalL2PagetableLocation);
    kprintf("KERNEL_TOP: 0x%x, KERNEL_BASE: 0x%x\n", KERNEL_TOP, KERNEL_BASE);

    mmu_started = true;
}



