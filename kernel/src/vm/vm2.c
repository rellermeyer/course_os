#include <vm2.h>
#include <stdbool.h>
#include <stdio.h>
#include <constants.h>
#include <pmm.h>
#include <hardwareinfo.h>

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

// Finds the location of an l2pt given an l1pt coarse entry.
static inline struct L2PageTable * find_l2pt (L1PagetableEntry * l1ptEntry){
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
            TRACE("[MEM DEBUG] Remapping l1 page located at 0x%x", virtual);
        } else {
            // The entry is already mapped
            FATAL("Request for already mapped address denied");
        }
    }

    kernell1PageTable->entries[l1pt_index(virtual)] = entry;

    if (remap && remapped) {
        // TODO: partial flush
        vm2_flush_caches();
    }

    return remapped;
}

void vm2_flush_caches() {
    TRACE("[MEM DEBUG] Flushing caches");
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

// http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/DDI0301H_arm1176jzfs_r0p7_trm.pdf#page=219
void vm2_flush_caches_of_ASID(uint8_t id) {
    asm volatile (
    "mcr p15, 0, %0, c8, c7, 2" // Invalidate TLB Entry on ASID Match
    :: "r" (id));
}

void vm2_set_user_pagetable(struct L1PageTable * l1) {
    // http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/DDI0301H_arm1176jzfs_r0p7_trm.pdf#page=360
    asm volatile ("MCR p15, 0, %0, c2, c0, 0\n" :: "r" (l1));     // Set Translation base address 0
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
            FATAL("Board type unsupported by VM2\n");
    }

    INFO("Using memory size 0x%x", available_RAM);

    /// Unmap the 1:1 mapped first megabyte which startup.s created to boot to a higher half kernel.
    /// We don't need it anymore!
    kernell1PageTable->entries[0] = (L1PagetableEntry){0};

    /// Map the entire gigabyte (or less on some boards, but never more) physical ram to virtual 2GB-3GB.
    /// this includes the kernel, kernel stack, kernel pagetables, process pagetables, pmm etc.
    for (size_t i = 0; i < (size_t)available_RAM; i += Mebibyte) {
        vm2_l1_map_physical_to_virtual(kernell1PageTable, (union L1PagetableEntry) {
                .section.type = 2,
                .section.accessPermissions = 1,
                .section.base_address = l1pt_base_address(i),
        }, KERNEL_VIRTUAL_OFFSET + i, true);
    }

    pmm_init(KERNEL_PMM_BASE, PMM_TOP);

    vm2_set_user_pagetable(NULL);

    vm2_flush_caches();

    mmu_started = true;
}

void *vm2_allocate_page(struct L1PageTable *l1pt, size_t virtual, bool remap, struct PagePermission perms,
                        struct L2PageTable **created_l2pt) {
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

            // Return the allocated l2pt
            if(created_l2pt != NULL){
                *created_l2pt = l2;
            }

            /** Fallthrough **/
        case 1:;
            struct Page * page = pmm_allocate_page();
            // There already is a coarse pagetable
            if(l2 == NULL) {
                l2 = find_l2pt(l1Entry);
            }
            union L2PagetableEntry * l2Entry = &l2->entries[l2pt_index(virtual)];

            if (l2Entry->entry != 0 && !remap) {
                FATAL("Overwriting entry in l2pt : 0x%x", virtual);
            }  else if (l2Entry->entry != 0 && remap) {
                TRACE("[MEM DEBUG] Remapping l2 page located at 0x%x", virtual);
            }

            // Set up perms correctly
            int accessPerms = 0;
            int accessExtended = 0;
            bool global = false;

            switch (perms.access) {
                case KernelRW:
                    accessPerms = 0b01;
                    accessExtended = 0;
                    global = true;
                    break;
                case KernelRO:
                    accessPerms = 0b01;
                    accessExtended = 1;
                    global = true;
                    break;
                case UserRO:
                    accessPerms = 0b10;
                    accessExtended = 0;
                    global = false;
                    break;
                case UserRW:
                    accessPerms = 0b11;
                    accessExtended = 0;
                    global = false;
                    break;
                default:
                    WARN("[MEM DEBUG] No access permissions specified, falling back to No access ");
                    break;
            }

            *l2Entry = (union L2PagetableEntry) {
                .smallpage = {
                    .type = 2 + !perms.executable,
                    .bufferable = 0,
                    .cachable = 0,
                    .notglobal = !global,
                    .accessPermissions = accessPerms,
                    .accessExtended = accessExtended,
                    .base_address = l2pt_base_address(((size_t)VIRT2PHYS(page))),
                },
            };

            if(remap) {
                vm2_flush_caches();
            }

            // make sure that if we didn't allocate a new pt, we set created_l2pt it to null
            if(created_l2pt != NULL && *created_l2pt != NULL){
                *created_l2pt = NULL;
            }

            return page;
        default:
            // This is a (super)section and we can't make it a coarse pagetable. Error.
            WARN("[MEM DEBUG] L1 Entry is a section or super section, can't map a page there");
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
