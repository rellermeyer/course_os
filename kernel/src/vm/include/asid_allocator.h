#ifndef TLB_CACHE_ID_ALLOCATOR
#define TLB_CACHE_ID_ALLOCATOR

#include <stdint.h>
#include <stdbool.h>

/// Terminology
/// * asid: Hardware process id used for the pagetable cache.
/// * cache_iteration: Which iteration of the cache we are on, determines if we should flush the cache on a process switch or not.
/// * asid_descriptor: A pair of above two numbers.

struct ASIDDescriptor {
    uint8_t tlb_cache_id;
    uint8_t cache_iteration;
};

/// Is set to true whenever all tlb_cache_ids are used up.
bool tlb_everything_allocated;

/**
 * Requests a tlb_descriptor.
 *
 *
 * Allocates you a new tlb_id,
 * if [tlb_everything_allocated] is true you should flush caches.
 * If [tlb_everything_allocated] is false, you don't have to flush caches.
 * @returns a TLBDescriptor
 */
struct ASIDDescriptor asid_request_descriptor();

/**
 * Updates and checks a tlb_descriptor.
 *
 * It checks if your saved cache_iteration corresponds with the current one.
 * If it doesn't it will update both, and return true.
 * If it does it won't update and return false.
 * @param desc the tlb_descriptor to check.
 * @returns whether you should flush caches or not.
 */
bool asid_check_and_update(struct ASIDDescriptor* desc);

/**
 * Sets a specific ASID for the current core.
 * @param id the ASID to set.
 */
void asid_set(uint8_t id);
#endif
