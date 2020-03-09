// Adapted from https://github.com/CCareaga/heap_allocator


#include <allocator.h>
#include <stdio.h>
#include <test.h>

int offset = sizeof(uint32_t) * 2;
uint32_t overhead = sizeof(footer_t) + sizeof(node_t);

// ========================================================
// this function initializes a new heap structure, provided
// an empty heap struct, and a place to start the heap
//
// NOTE: this function uses HEAP_INIT_SIZE to determine
// how large the heap is so make sure the same constant
// is used when allocating memory for your heap!
// ========================================================
void create_heap(heap_t *heap, uint32_t start) {
    // first we create the initial region, this is the "wilderness" chunk
    // the heap starts as just one big chunk of allocatable memory
    node_t *init_region = (node_t *) start;
    init_region->hole = 1;
    init_region->size = (HEAP_INIT_SIZE) - sizeof(node_t) - sizeof(footer_t);

    create_foot(init_region); // create a foot (size must be defined)

    // now we add the region to the correct bin and setup the heap struct
    add_node(heap->bins[get_bin_index(init_region->size)], init_region);

    heap->start = start;
    heap->end = start + HEAP_INIT_SIZE;

#ifdef MEM_DEBUG
    heap->bytes_allocated = 0;
#endif
}

// ========================================================
// this is the allocation function of the heap, it takes
// the heap struct pointer and the size of the chunk we
// want. this function will search through the bins until
// it finds a suitable chunk. it will then split the chunk
// if neccesary and return the start of the chunk
// ========================================================
void *heap_alloc(heap_t *heap, uint32_t size) {
    // first get the bin index that this chunk size should be in
    int index = get_bin_index(size);
    // now use this bin to try and find a good fitting chunk!
    bin_t *temp = (bin_t *) heap->bins[index];

    node_t *found = get_best_fit(temp, size);

    // while no chunk if found advance through the bins until we
    // find a chunk or get to the wilderness
    while (found == NULL) {
        if (index + 1 >= BIN_COUNT)
            return NULL;

        temp = heap->bins[++index];
        found = get_best_fit(temp, size);
    }

    // if the difference between the found chunk and the requested chunk
    // is bigger than the overhead (metadata size) + the min alloc size
    // then we should split this chunk, otherwise just return the chunk
    if ((found->size - size) > (overhead + MIN_ALLOC_SZ)) {
        // do the math to get where to split at, then set its metadata
        node_t *split = (node_t *)(((char *) found + overhead) + size);
        split->size = found->size - size - (overhead);
        split->hole = 1;

        create_foot(split); // create a footer for the split

        // now we need to get the new index for this split chunk
        // place it in the correct bin
        int new_idx = get_bin_index(split->size);
        add_node(heap->bins[new_idx], split);

        found->size = size; // set the found chunks size
        create_foot(found); // since size changed, remake foot
    }


    found->hole = 0; // not a hole anymore
    remove_node(heap->bins[index], found); // remove it from its bin

    // these following lines are checks to determine if the heap should
    // be expanded or contracted
    // ==========================================
    node_t *wild = get_wilderness(heap);
    if (wild->size < MIN_WILDERNESS) {
        int success = expand(heap, 0x1000);
        if (success == 0) {
            return NULL;
        }
    }
    else if (wild->size > MAX_WILDERNESS) {
        contract(heap, 0x1000);
    }
    // ==========================================

    // since we don't need the prev and next fields when the chunk
    // is in use by the user, we can clear these and return the
    // address of the next field
    found->prev = NULL;
    found->next = NULL;


#ifdef MEM_DEBUG
    heap->bytes_allocated += found->size;
    TRACE("[MEM DEBUG] ALLOC %i bytes at 0x%x", found->size, &found->next);
#endif

    return &found->next;
}

// ========================================================
// this is the free function of the heap, it takes the
// heap struct pointer and the pointer provided by the
// heap_alloc function. the given chunk will be possibly
// coalesced  and then placed in the correct bin
// ========================================================
void heap_free(heap_t *heap, void *p) {
    if (p == NULL) {
        return;
    }


    bin_t *list;
    footer_t *new_foot, *old_foot;

    node_t *head = (node_t *) ((char *) p - offset);

#ifdef MEM_DEBUG
    heap->bytes_allocated -= head->size;
    TRACE("[MEM DEBUG] FREE %i bytes", head->size);
#endif

    if (head == (node_t *) (uintptr_t) heap->start) {
        head->hole = 1;
        add_node(heap->bins[get_bin_index(head->size)], head);
        return;
    }

    node_t *next = (node_t *) ((char *) get_foot(head) + sizeof(footer_t));
    footer_t *f = (footer_t *) ((char *) head - sizeof(footer_t));
    node_t *prev = f->header;

    if (prev->hole) {
        list = heap->bins[get_bin_index(prev->size)];
        remove_node(list, prev);

        prev->size += overhead + head->size;
        new_foot = get_foot(head);
        new_foot->header = prev;

        head = prev;
    }

    if (next->hole) {
        list = heap->bins[get_bin_index(next->size)];
        remove_node(list, next);

        head->size += overhead + next->size;

        old_foot = get_foot(next);
        old_foot->header = 0;
        next->size = 0;
        next->hole = 0;

        new_foot = get_foot(head);
        new_foot->header = head;
    }

    head->hole = 1;
    add_node(heap->bins[get_bin_index(head->size)], head);
}


// these are left here to implement contraction / expansion
uint32_t expand(heap_t *heap, uint32_t sz) {
    TRACE("[MEM DEBUG] Trying to expand");
    return 0; // fail for now
}

void contract(heap_t *heap, uint32_t sz) {

}

// ========================================================
// this function is the hashing function that converts
// size => bin index. changing this function will change
// the binning policy of the heap. right now it just
// places any allocation < 8 in bin 0 and then for anything
// above 8 it bins using the log base 2 of the size
// ========================================================
uint32_t get_bin_index(uint32_t sz) {
    int index = 0;
    sz = sz < 4 ? 4 : sz;

    while (sz >>= 1) index++;
    index -= 2;

    if (index > BIN_MAX_IDX) index = BIN_MAX_IDX;

    return index;
}

// ========================================================
// this function will create a footer given a node
// the node's size must be set to the correct value!
// ========================================================
void create_foot(node_t *head) {
    footer_t *foot = get_foot(head);
    foot->header = head;
}


// ========================================================
// this function will get the footer pointer given a node
// ========================================================
footer_t *get_foot(node_t *node) {
    return (footer_t *) ((char *) node + sizeof(node_t) + node->size);
}

// ========================================================
// this function will get the wilderness node given a
// heap struct pointer
//
// NOTE: this function banks on the heap's end field being
// correct, it simply uses the footer at the end of the
// heap because that is always the wilderness
// ========================================================
node_t *get_wilderness(heap_t *heap) {
    footer_t *wild_foot = (footer_t *) ((char *) heap->end - sizeof(footer_t));
    return wild_foot->header;
}

uint32_t get_alloc_size(void * ptr) {
    node_t *head = (node_t *) (ptr - offset);
    return head->size;
}
