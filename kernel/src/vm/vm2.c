#include <vm2.h>
#include <stdbool.h>
#include <stdio.h>
#include <constants.h>
#include <pmm.h>
#include <hardwareinfo.h>
#include <string.h>

struct L1PageTable * kernell1PageTable = (struct L1PageTable *) VirtualL1PagetableLocation; // NOLINT(cppcoreguidelines-interfaces-global-init) (defined in linker script)
bool mmu_started = false;

static inline size_t l1pt_index(size_t address) {
    return address >> 20u;
}

static inline uint8_t l2pt_index(size_t address) {
    return (address & 0x000ff000u) >> 12u;
}

static inline size_t l1pt_base_address(size_t address) {
    // For l1 they are the same
    return l1pt_index(address);
}

static inline size_t l2pt_base_address(size_t address) {
    return address >> 12u;
}

// TODO: More descriptive name
// Finds the location of an l2pt given an l1pt coarse entry.
static inline struct L2PageTable * l2pt_in_l1 (L1PagetableEntry * l1ptEntry){
    if(l1ptEntry->coarse.type == 1) {
        return (struct L2PageTable *) PHYS2VIRT(l1ptEntry->coarse.base_address << 10u);
    } else {
        FATAL("Can't get L2 address from non-coarse L1 Entry");
    }
}

bool vm2_l1_map_physical_to_virtual(struct L1PageTable * pt, union L1PagetableEntry entry, size_t virtual, bool remap) {

    bool remapped = false;

    if (pt->entries[l1pt_index(virtual)].entry != 0) {
        if (remap) {
            remapped = true;
        } else {
            // The entry is already mapped
            FATAL("Request for already mapped address denied");
        }
    }

    kernell1PageTable->entries[l1pt_index(virtual)] = entry;

    return remapped;
}

void vm2_flush_caches() {
    //TODO: Only a subset of these instructions are necessary
    asm volatile (
    "// invalidate caches\n"
    "mcr p15, 0, %0, c7, c5, 0\n"
    "// invalidate tlb\n"
    "mcr p15, 0, %0, c8, c7, 0\n"
    "// data sync barrier\n"
    "mcr p15, 0, %0, c7,c10, 4\n"

    "mcr p15, 0, %0, c7, c10, 4\n"
    "mcr p15, 0, %0, c7, c10, 5\n"
    "mcr p15, 0, %0, c7, c14, 0\n"
    :: "r"(0x0));
}

// Starts the actual MMU after this function we live in Virtual Memory
void vm2_start() {
    size_t available_RAM;

    switch (get_hardwareinfo()->boardType) {
        case RaspberryPiZero:
            available_RAM = 512 * Mebibyte;
            break;
        case RaspBerryPiTwo:
            available_RAM = 1 * Gibibyte;
            break;
        default:
            // TODO: memory detection? Or just not bother.
            kprintf("Board type unsupported by VM2\n");
            panic();
    }

    INFO("Using memory size 0x%x", available_RAM);

    /// Unmap the 1:1 mapped first megabyte which startup.s created to boot to a higher half kernel.
    /// We don't need it anymore!
//    kernell1PageTable->entries[0] = (L1PagetableEntry){0};

    /// Map the entire gigabyte (or less on some boards, but never more) physical ram to virtual 2GB-3GB.
    /// this includes the kernel, kernel stack, kernel pagetables, process pagetables, pmm etc.
    for (size_t i = 0; i < (size_t)available_RAM; i += Mebibyte) {
        vm2_l1_map_physical_to_virtual(kernell1PageTable, (union L1PagetableEntry) {
                .section.type = 2,
                .section.accessPermissions = 1,
                .section.base_address = l1pt_base_address(i),
        }, KERNEL_VIRTUAL_OFFSET + i, true);
    }

    pmm_init(KERNEL_PMM_BASE, KERNEL_VIRTUAL_OFFSET + available_RAM);


    vm2_flush_caches();
    mmu_started = true;
}

void * vm2_allocate_kernel_page(struct L1PageTable * l1pt, size_t virtual, bool executable) {
    L1PagetableEntry * l1Entry = &l1pt->entries[l1pt_index(virtual)];
    struct L2PageTable * l2 = NULL;

    switch (l1Entry->section.type ) {
        case 0:;
            // Allocate coarse/l2 pagetable
            l2 = pmm_allocate_l2_pagetable();
            *l1Entry = (L1PagetableEntry) {
                .coarse = {
                    .type = 1,
                    .base_address = ((size_t)VIRT2PHYS((size_t)l2)) >> 10u,
                },
            };
            /** Fallthrough **/
        case 1:;
            struct Page * page = pmm_allocate_page();
            // There already is a coarse pagetable
            if(l2 == NULL) {
                l2 = l2pt_in_l1(l1Entry);
            }
            union L2PagetableEntry * l2Entry = &l2->entries[l2pt_index(virtual)];

            *l2Entry = (union L2PagetableEntry) {
                .smallpage = {
                    .type = 2 + !executable,
                    .bufferable = 0,
                    .cachable = 0,
                    .accessPermissions = 1,
                    .accessExtended = 0,
                    .base_address = l2pt_base_address(((size_t)VIRT2PHYS(page))),
                },
            };

            return page;
        default:
            // This is a (super)section and we can't make it a coarse pagetable. Error.
            return NULL;
    }
}

size_t vm2_map_peripheral(size_t physical, size_t n_mebibytes) {
    static size_t mmio_current_top = KERNEL_MMIO_BASE;

    mmio_current_top -= (n_mebibytes * Mebibyte);

    for (size_t i = 0; i < n_mebibytes; i++)  {

        kernell1PageTable->entries[l1pt_index(mmio_current_top)] = (L1PagetableEntry){
            .section.type = 2,
            .section.accessPermissions = 1,
            .section.base_address = l1pt_base_address(physical),
        };

        mmio_current_top += Mebibyte;
        physical += Mebibyte;
    }

    mmio_current_top -= (n_mebibytes * Mebibyte);

    return mmio_current_top;
}



