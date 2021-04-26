/**
 * buddy system page frame allocator
 * modified from: https://github.com/block8437/buddyalloc
 */

#include <allocator.h>
#include <string.h>
#include <stdio.h>

#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) < (b) ? (b) : (a))

#define GET_NEXT(head) *(size_t*)(head)
#define BUDDY_CHECK(a, b, size) ((a ^ size) == (b))

// Debug toggle
static bool trace_memory = false;

// ========================================================
// Set the trace memory variable
// ========================================================
void set_trace_memory(const bool value) {
    trace_memory = value;
}

bool get_trace_memory(void) {
    return trace_memory;
}

// Add a free chunk into a certain order
void buddy_add_free_item(buddy_allocator * buddy, size_t address, size_t order, bool new) {
    size_t head = buddy->free_lists[order - MIN_ORDER];
    GET_NEXT(address) = 0;
    size_t size = 1 << order;

    if (new || head == 0) {
        // Fresh address, push to the head.
        GET_NEXT(address) = head;
        buddy->free_lists[order - MIN_ORDER] = address;
        return;
    }

    // Old address with a nonempty list, lets find some buddies!
    size_t prev = 0;
    while (!BUDDY_CHECK(head, address, size)) {
        // We reached the end of the list, add it here and return
        if (GET_NEXT(head) == 0) {
            GET_NEXT(head) = address;
            return;
        }

        prev = head;
        head = GET_NEXT(head);
    }

    // We found buddies
    if (prev != 0) {
        // Remove this item from it's old list
        GET_NEXT(prev) = GET_NEXT(head);
    } else {
        buddy->free_lists[order - MIN_ORDER] = GET_NEXT(head);
    }

    buddy_add_free_item(buddy, min(head, address), order + 1, false);
}

// Allocate a chunk from a buddy allocator
void* buddy_alloc_helper(buddy_allocator * buddy, size_t size) {
    // Round up by getting highest order and calculating the size form that
    int original_order = (32 - __builtin_clz(size - 1));

    // Not enough size for this request;
    if (original_order >= MAX_ORDER) {
        return NULL;
    }

    if (original_order < MIN_ORDER) {
        original_order = MIN_ORDER;
    }

    size_t want_size = 1 << original_order;

    // Find the smallest order with space
    for ( int order = original_order; order < MAX_ORDER; order++ ) {
        if (buddy->free_lists[order - MIN_ORDER] != 0 ) {
            // Pop the head
            size_t address = buddy->free_lists[order - MIN_ORDER];
            buddy->free_lists[order - MIN_ORDER] = GET_NEXT(address);

            // Try to return it's buddies
            size_t found_size = 1 << order;

            if (found_size != want_size) {
                // Keep halving the found size, returning the second parts of the halves into
                // the free list as we go, until we find the right size.

                while (found_size > want_size) {
                    found_size = found_size >> 1;
                    buddy_add_free_item(buddy, address + found_size,
                                        32 - __builtin_clz(found_size - 1), true);
                }
            }

            return (void*) address;
        }
    }

    // Didn't find anything :(
    return NULL;
}

void * buddy_alloc(buddy_allocator * buddy, size_t size) {
    buddy_allocation * start = buddy_alloc_helper(buddy,
                                                  size + sizeof(buddy_allocation_header));
    start->header.size = size;

    if (get_trace_memory()) {
        buddy->bytes_allocated += start->header.size;
        TRACE("[MEM DEBUG] ALLOC %i bytes at 0x%x", start->header.size, &start->allocation);
    }

    return &start->allocation;
}

// Return a chunk of memory ack into the buddy allocator
void buddy_dealloc_helper(buddy_allocator * buddy, size_t address, size_t size) {

    int order = 32 - __builtin_clz(size - 1);
    if (order < MIN_ORDER) {
        order = MIN_ORDER;
    }
    if (order >= MAX_ORDER) {
        FATAL("Deallocation larger than MAX_ORDER");
    }


    buddy_add_free_item(buddy, address, order, false);
}

void buddy_dealloc(buddy_allocator * buddy, size_t address) {
    if (address == (size_t)NULL) {
        return;
    }

    // get the header
    buddy_allocation_header * header = (buddy_allocation_header *)
        (address - sizeof(buddy_allocation_header));
    size_t size = header->size;

    // deallocate from the start of the header to the end of the
    // allocation (so add 4 bytes to deallocate)
    buddy_dealloc_helper(buddy, (size_t)header, size + sizeof(buddy_allocation_header));

    if (get_trace_memory()) {
        buddy->bytes_allocated -= size;
        TRACE("[MEM DEBUG] FREE %i bytes", size);
    }
}

// Prints out the current status of the allocator
void buddy_status(buddy_allocator * buddy) {
    for (int order = MIN_ORDER; order < MAX_ORDER; order++ ) {
        INFO("Order %d", order);

        size_t head = buddy->free_lists[order - MIN_ORDER];

        while (head != 0) {
            INFO("| 0x%x", head);
            head = GET_NEXT(head);
        }
    }
}

/// Finds size of an allocation
size_t get_alloc_size(size_t address) {
    buddy_allocation_header * header = (buddy_allocation_header*)
                                        (address - sizeof(buddy_allocation_header));
    return header->size;

}

// Initialize a buddy allocator
buddy_allocator * buddy_init(buddy_allocator *address, size_t size) {
    // If we don't have enough space for a header and the smallest chunk of space, ignore.
    if ( size <= sizeof(buddy_allocator) + (1 << MIN_ORDER)) {
        return NULL;
    }

    // Take the first chunk of space for the header
    buddy_allocator * buddy = (buddy_allocator *) address;
    address += sizeof(buddy_allocator);
    size -= sizeof(buddy_allocator);

    // Setup the header
    memset(buddy, 0, sizeof(buddy_allocator));
    buddy->base = (size_t) address;

    // Carve out as many chunks of space as possible.
    while (size > (1U << MIN_ORDER)) {
        for(int order = MAX_ORDER - 1; order >= MIN_ORDER; order--) {
            // Find the largest order to cut out of the size.
            if ( size >= (1U << order) ) {
                buddy_add_free_item(buddy, (size_t)address, order, true);

                address += 1U << order;
                size -= 1U << order;
                break;
            }
        }
    }

    if (get_trace_memory()) {
        buddy->bytes_allocated = 0;
    }

    return buddy;
}
