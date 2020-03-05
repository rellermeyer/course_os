#include <vas2.h>
#include <stdlib.h>
#include <pagealloc2.h>

struct vas2 * create_vas() {
    struct vas2 * newvas = kmalloc(sizeof(struct vas2));

    *newvas = (struct vas2) {
        .tlbDescriptor = request_tlb_descriptor(),
        .l1PageTable = pagealloc_allocate_l1_pagetable(),
    };

    return newvas;
}


void switch_to_vas(struct vas2 * vas) {
    
}

void free_vas() {
    
}

