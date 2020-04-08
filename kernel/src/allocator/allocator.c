/**
 * buddy system page frame allocator
 * modified from :
 */

#include <allocator.h>
#include <string.h>
#include <stdio.h>

#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) < (b) ? (b) : (a))

#define GET_NEXT(head) *(uintptr_t*)(head)

// Determines whether or not a and b are buddies of size size.
bool buddy_check(buddy_alloc_t* buddy, uintptr_t a, uintptr_t b, size_t size) {
    uintptr_t lower = min(a, b) - buddy->base;
    uintptr_t upper = max(a, b) - buddy->base;

    // Calculate upper from lower and check.
    return (lower ^ size) == upper;
}

// Add a free chunk into a certain order
void buddy_add_free_item(buddy_alloc_t* buddy, uintptr_t address, size_t order, bool new) {
    uintptr_t head = buddy->free_lists[order - MIN_ORDER];
    GET_NEXT(address) = 0;
    size_t size = 1 << order;

    if ( !new && head != 0) {
        // Old address with a nonempty list, lets find some buddies!
        uintptr_t prev = 0;

        // Kind of ugly but w\e
        while ( true ) {
            // Check for buddies
            if ( buddy_check(buddy, head, address, size) ) {
                // We found buddies

                if ( prev != 0 ) {
                    // Remove this item from it's old list
                    GET_NEXT(prev) = GET_NEXT(head);
                }
                else {
                    buddy->free_lists[order - MIN_ORDER] = GET_NEXT(head);
                }

                buddy_add_free_item(buddy, min(head, address), order + 1, false);
                return;
            }

            if ( GET_NEXT(head) == 0 ) {
                // We reached the end of the list, add it here.
                GET_NEXT(head) = address;
                break;
            }

            prev = head;
            head = GET_NEXT(head);
        }
    }
    else {
        // Fresh address, push to the head.
        GET_NEXT(address) = head;
        buddy->free_lists[order - MIN_ORDER] = address;
    }
}

// Allocate a chunk from a buddy allocator
void* buddy_alloc(buddy_alloc_t* buddy, size_t size) {
    // Round up by getting highest order and calculating the size form that
    int original_order = (32 - __builtin_clz(size - 1));

    // Not enough size for this request;
    if ( original_order >= MAX_ORDER ) {
        return NULL;
    }

    size_t want_size = 1 << original_order;

    // Find the smallest order with space
    for ( int order = original_order; order < MAX_ORDER; order++ ) {
        if ( buddy->free_lists[order - MIN_ORDER] != 0 ) {
            // Pop the head
            uintptr_t address = buddy->free_lists[order - MIN_ORDER];
            buddy->free_lists[order - MIN_ORDER] = GET_NEXT(address);

            // Try to return it's buddies
            size_t found_size = 1 << order;

            if ( found_size != want_size ) {
                // Keep halving the found size, returning the second parts of the halves into
                // the free list as we go, until we find the right size.

                while ( found_size > want_size ) {
                    found_size = found_size >> 1;
                    buddy_add_free_item(buddy, address + found_size, 32 - __builtin_clz(found_size - 1), true);
                }
            }

            return (void*) address;
        }
    }

    // Didin't find anything :(
    return NULL;
}

// Return a chunk of memory ack into the buddy allocator
void buddy_dealloc(buddy_alloc_t* buddy, uintptr_t address, size_t size) {
    int order = 32 - __builtin_clz(size - 1);
    buddy_add_free_item(buddy, address, order, false);
}

// Prints out the current status of the allocator
void buddy_status(buddy_alloc_t* buddy) {
    for ( int order = MIN_ORDER; order < MAX_ORDER; order++ ) {
        //printf("Order %d\n", order);

        uintptr_t head = buddy->free_lists[order - MIN_ORDER];

        while ( head != 0 ) {
            //printf("| %08x\n", head);
            head = GET_NEXT(head);
        }
    }
}

// Finds size of an allocation
unsigned int get_alloc_size(void* ptr) {

}

// Initialize a buddy allocator
buddy_alloc_t* buddy_init(uintptr_t address, size_t size) {
    // If we don't have enough space for a header and the smallest chunk of space, ignore.
    if ( size <= sizeof(buddy_alloc_t) + (1 << MIN_ORDER) ) {
        return NULL;
    }

    // Take the first chunk of space for the header
    buddy_alloc_t* buddy = (buddy_alloc_t*) address;
    address += sizeof(buddy_alloc_t);
    size -= sizeof(buddy_alloc_t);

    // Setup the header
    memset(buddy, 0, sizeof(buddy_alloc_t));
    buddy->base = address;

    // Carve out as many chunks of space as possible.
    while ( size > (1 << MIN_ORDER) ) {
        for ( int order = MAX_ORDER - 1; order >= MIN_ORDER; order-- ) {
            // Find the largest order to cut out of the size.
            if ( size >= (1 << order) ) {
                buddy_add_free_item(buddy, address, order, true);

                address += 1 << order;
                size -= 1 << order;
                break;
            }
        }
    }

    return buddy;
}