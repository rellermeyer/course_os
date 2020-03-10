#include <asid_allocator.h>
#include <stdint.h>
#include <stdbool.h>
#include <vm2.h>

uint8_t curr = 0;
uint8_t allocated_ids[256] = {0};

bool tlb_everything_allocated = false;

// TODO: Ensure atomicity
struct ASIDDescriptor asid_request_descriptor() {
    uint8_t cache_iteration = ++(allocated_ids[curr]);
    uint8_t tlb_cache_id = curr++;

    if(tlb_cache_id == 255) {
        tlb_everything_allocated = true;
    }

    struct ASIDDescriptor res = (struct ASIDDescriptor) {
        .cache_iteration = cache_iteration,
        .asid = tlb_cache_id,
    };

    return res;
}

// TODO: Ensure atomicity
bool asid_check_and_update(struct ASIDDescriptor* desc) {
    if(desc->cache_iteration != allocated_ids[desc->asid]) {
        desc->cache_iteration = ++(allocated_ids[desc->asid]);
        return true;
    }

    return false;
}

void asid_set(uint8_t id) {

    DATA_SYNC_BARRIER()

    asm volatile (
        "mrc p15, 0, r1, c13, c0, 1 \n" // Read Context ID (c13) register to R1
        "and r1, r1, #0xffffff00\n"
        "orr r1, %0, r1\n"
        "mcr p15, 0, r1, c13, c0, 1\n" // Write it back with the ASID set
        ::"r"(id)
        : "r1"
    );
}
