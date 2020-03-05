#include <tlb_cache_id_allocator.h>
#include <stdint.h>
#include <stdbool.h>

uint8_t curr = 0;
uint32_t allocated_ids[256] = {0};

struct TLBDescriptor request_tlb_descriptor() {
    uint32_t cache_iteration = ++(allocated_ids[curr]);
    uint32_t tlb_cache_id = curr++;

    struct TLBDescriptor res = (struct TLBDescriptor) {
        .cache_iteration = cache_iteration,
        .tlb_cache_id = tlb_cache_id,
    };

    return res;
}


bool get_and_update(struct TLBDescriptor* desc) {
    if(desc->cache_iteration != allocated_ids[desc->tlb_cache_id]) {
        desc->cache_iteration = ++(allocated_ids[desc->tlb_cache_id]);
        return true;
    }

    return false;
}
