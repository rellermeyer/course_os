#ifndef TLB_CACHE_ID_ALLOCATOR
#define TLB_CACHE_ID_ALLOCATOR

#include <stdint.h>
#include <stdbool.h>

/// Terminology
/// * tlb_cache_id: Hardware process id used for the pagetable cache.
/// * cache_iteration: Which iteration of the cache we are on, determines if we should flush the cache on a process switch or not.
/// * tlb_descriptor: A pair of above two numbers.

struct TLBDescriptor {
    uint32_t tlb_cache_id;
    uint32_t cache_iteration;
};

struct TLBDescriptor request_tlb_descriptor();

/// Updates the current tlb_descriptor you have.
/// If it was able to, it gives you the same tlb_cache_id that you already had.
/// Then it returns false.
///
/// If all of those were allocated it updates your id and returns true.
/// When this function returns true, flush the caches.
bool get_and_update(struct TLBDescriptor* desc);


#endif
