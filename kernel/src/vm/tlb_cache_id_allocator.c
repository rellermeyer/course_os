#include <tlb_cache_id_allocator.h>
#include <stdint.h>
#include <stdbool.h>

uint8_t curr = 0;
uint8_t allocated_ids[256] = {0};

bool tlb_everything_allocated = false;

// TODO: Ensure atomicity
struct TLBDescriptor request_tlb_descriptor() {
    uint8_t cache_iteration = ++(allocated_ids[curr]);
    uint8_t tlb_cache_id = curr++;

    if(tlb_cache_id == 255) {
        tlb_everything_allocated = true;
    }

    struct TLBDescriptor res = (struct TLBDescriptor) {
        .cache_iteration = cache_iteration,
        .tlb_cache_id = tlb_cache_id,
    };

    return res;
}

// TODO: Ensure atomicity
bool check_and_update(struct TLBDescriptor* desc) {
    if(desc->cache_iteration != allocated_ids[desc->tlb_cache_id]) {
        desc->cache_iteration = ++(allocated_ids[desc->tlb_cache_id]);
        return true;
    }

    return false;
}
