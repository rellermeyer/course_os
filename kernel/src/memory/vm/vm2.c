#include <vm2.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct L1PageTable * l1PageTable = (struct L1PageTable *) L1PagetableLocation;
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

    if (l1PageTable->entries[l1pt_index(virtual)].entry != 0){
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
            .section.base_address = (physical >> 20u) << 20u,
    };

    l1PageTable->entries[l1pt_index(virtual)] = entry;
}

void vm2_map_nmegabytes_1to1(size_t address, size_t n) {
    // Iterate over the address in 1 MiB chunks.
    for (; address < (address + 0x100000u * n) ; address += 0x100000u) {
        kprintf("mapping address 0x%x 1:1\n", address);
        vm2_map_virtual_to_physical_l1(address, address);
    }
}

// Prepares the pagetable and add
void vm2_prepare() {
    // Clear the pagetable
    memset(l1PageTable, 0, sizeof(struct L1PageTable));


    vm2_map_nmegabytes_1to1(0, 1);

}

// Starts the actual MMU after this function we live in Virtual Memory
void vm2_start() {

    mmu_started = true;
}
