#ifndef TLB_CACHE_ID_ALLOCATOR
#define TLB_CACHE_ID_ALLOCATOR

#include <stdbool.h>
#include <stdint.h>

/// We have made a special allocator to keep track of all the ASIDs used and to determine if we
/// would need to flush the caches upon a context switch. This should greatly reduce the amount of
/// cache flushes needed and optimizes the usage of ASIDs.
///
/// Designed by:
/// Victor Roest            <victor@xirion.net>
///
/// Implemented by:
/// Jonathan Donszelmann    <jonabent@gmail.com>
/// Victor Roest            <victor@xirion.next>


/// Terminology
/// * asid: Hardware process id used for the pagetable cache.
/// * cache_iteration: Which iteration of the cache we are on, determines if we should flush the
/// cache on a process switch or not.
/// * asid_descriptor: A pair of above two numbers.

/// Each ASID Descriptor has 2 fields:
/// asid: the actual ASID used for this process.
/// cache_iteration: On which iteration of the cache we are (used to determine if a cache flush is
/// needed).
struct ASIDDescriptor {
    uint8_t asid;
    uint8_t cache_iteration;
};

/// Is set to true whenever all tlb_cache_ids are used up.
extern bool tlb_everything_allocated;

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
bool asid_check_and_update(struct ASIDDescriptor * desc);

/**
 * Sets a specific ASID for the current core.
 * @param id the ASID to set.
 */
void asid_set(uint8_t id);

#endif
