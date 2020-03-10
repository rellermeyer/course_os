#include <vas2.h>
#include <stdlib.h>
#include <string.h>
#include <pmm.h>

/// FIXME:
/// WARNING: This code is untested, and quite hard to even make tests for.
/// This should just be used in a scheduler to see if it works.

struct vas2 * create_vas() {
    struct vas2 * newvas = kmalloc(sizeof(struct vas2));

    *newvas = (struct vas2) {
        .tlbDescriptor = asid_request_descriptor(),
        .l1PageTable = pmm_allocate_l1_pagetable(),
        .l2tables = vpa_create(VAS2_INITIAL_PAGE_LIST_CAPACITY),
        .pages = vpa_create(VAS2_INITIAL_PAGE_LIST_CAPACITY),
    };

    // Copy the upper half of the kernelPageTable to any other page table.
    memcpy(newvas->l1PageTable, &kernell1PageTable->entries[0x800], sizeof(struct L1PageTable) / 2);

    return newvas;
}

void switch_to_vas(struct vas2 * vas) {
    if(asid_check_and_update(&vas->tlbDescriptor)) {
        vm2_flush_caches_of_ASID(vas->tlbDescriptor.tlb_cache_id);
    }

    asid_set(vas->tlbDescriptor.tlb_cache_id);

    vm2_set_current_pagetable(vas->l1PageTable);
}

void free_vas(struct vas2 * vas) {
    vpa_free(vas->l2tables, (FreeFunc) pmm_free_l2_pagetable);
    vpa_free(vas->pages, (FreeFunc) pmm_free_page);
    pmm_free_l1_pagetable(vas->l1PageTable);
    kfree(vas);
}

void allocate_page(struct vas2 * vas, size_t address, bool executable) {
    struct PagePermission perms = (struct PagePermission) {
        .executable = executable,
        .access = UserRW,
    };

    struct L2PageTable * l2pt = NULL;

    void * page = vm2_allocate_page(vas->l1PageTable, address, false, perms, &l2pt);

    if (l2pt != NULL) {
        vpa_push(vas->l2tables, l2pt);
    }

    vpa_push(vas->pages, page);
}
