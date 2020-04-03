#include <unistd.h>
#include <allocator.h>

// Circularly Linked List methods
static void list_init(list_t *list) {
    list->prev = list;
    list->next = list;
}

static void list_push(list_t *list, list_t *entry) {
    list_t *prev = list->prev;
    entry->prev = prev;
    entry->next = list;
    prev->next = entry;
    list->prev = entry;
}

static void list_remove(list_t *entry) {
    list_t *prev = entry->prev;
    list_t *next = entry->next;
    prev->next = next;
    next->prev = prev;
}

static list_t *list_pop(list_t *list) {
    list_t *back = list->prev;
    if (back == list) return NULL;
    list_remove(back);
    return back;
}
/*
 * This maps from the index of a node to the address of memory that node
 * represents. The bucket can be derived from the index using a loop but is
 * required to be provided here since having them means we can avoid the loop
 * and have this function return in constant time.
 */
static uint8_t *ptr_for_node(size_t index, size_t bucket) {
    return base_ptr + ((index - (1 << bucket) + 1) << (MAX_ALLOC_LOG2 - bucket));
}

/*
 * This maps from an address of memory to the node that represents that
 * address. There are often many nodes that all map to the same address, so
 * the bucket is needed to uniquely identify a node.
 */
static size_t node_for_ptr(uint8_t *ptr, size_t bucket) {
    return ((ptr - base_ptr) >> (MAX_ALLOC_LOG2 - bucket)) + (1 << bucket) - 1;
}
/*
 * Given the index of a node, this returns the "is split" flag of the parent.
 */
static int parent_is_split(size_t index) {
    index = (index - 1) / 2;
    return (node_is_split[index / 8] >> (index % 8)) & 1;
}

/*
 * Given the index of a node, this flips the "is split" flag of the parent.
 */
static void flip_parent_is_split(size_t index) {
    index = (index - 1) / 2;
    node_is_split[index / 8] ^= 1 << (index % 8);
}

/*
 * Given the requested size passed to "malloc", this function returns the index
 * of the smallest bucket that can fit that size.
 */
static size_t bucket_for_request(size_t request) {
    size_t bucket = BUCKET_COUNT - 1;
    size_t size = MIN_ALLOC;

    while (size < request) {
        bucket--;
        size *= 2;
    }

    return bucket;
}

/*
 * The tree is always rooted at the current bucket limit. This call grows the
 * tree by repeatedly doubling it in size until the root lies at the provided
 * bucket index. Each doubling lowers the bucket limit by 1.
 */
static int lower_bucket_limit(size_t bucket) {
    while (bucket < bucket_limit) {
        size_t root = node_for_ptr(base_ptr, bucket_limit);
        uint8_t *right_child;

        /*
         * If the parent isn't SPLIT, that means the node at the current bucket
         * limit is UNUSED and our address space is entirely free. In that case,
         * clear the root free list, increase the bucket limit, and add a single
         * block with the newly-expanded address space to the new root free list.
         */
        if (!parent_is_split(root)) {
            list_remove((list_t *)base_ptr);
            list_init(&buckets[--bucket_limit]);
            list_push(&buckets[bucket_limit], (list_t *)base_ptr);
            continue;
        }

        /*
         * Otherwise, the tree is currently in use. Create a parent node for the
         * current root node in the SPLIT state with a right child on the free
         * list. Make sure to reserve the memory for the free list entry before
         * writing to it. Note that we do not need to flip the "is split" flag for
         * our current parent because it's already on (we know because we just
         * checked it above).
         */
        right_child = ptr_for_node(root + 1, bucket_limit);
        if (!update_max_ptr(right_child + sizeof(list_t))) {
            return 0;
        }
        list_push(&buckets[bucket_limit], (list_t *)right_child);
        list_init(&buckets[--bucket_limit]);

        /*
         * Set the grandparent's SPLIT flag so if we need to lower the bucket limit
         * again, we'll know that the new root node we just added is in use.
         */
        root = (root - 1) / 2;
        if (root != 0) {
            flip_parent_is_split(root);
        }
    }

    return 1;
}

void * alloc_buddy(size_t request) {
    size_t original_bucket, bucket;

    /*
     * Make sure it's possible for an allocation of this size to succeed. There's
     * a hard-coded limit on the maximum allocation size because of the way this
     * allocator works.
     */
    if (request + HEADER_SIZE > MAX_ALLOC) {
        return NULL;
    }

    /*
     * Initialize our global state if this is the first call to "malloc". At the
     * beginning, the tree has a single node that represents the smallest
     * possible allocation size. More memory will be reserved later as needed.
     */
    if (base_ptr == NULL) {
        base_ptr = max_ptr = (uint8_t *)sbrk(0);
        bucket_limit = BUCKET_COUNT - 1;
        update_max_ptr(base_ptr + sizeof(list_t));
        list_init(&buckets[BUCKET_COUNT - 1]);
        list_push(&buckets[BUCKET_COUNT - 1], (list_t *)base_ptr);
    }

    /*
     * Find the smallest bucket that will fit this request. This doesn't check
     * that there's space for the request yet.
     */
    bucket = bucket_for_request(request + HEADER_SIZE);
    original_bucket = bucket;

    /*
     * Search for a bucket with a non-empty free list that's as large or larger
     * than what we need. If there isn't an exact match, we'll need to split a
     * larger one to get a match.
     */
    while (bucket + 1 != 0) {
        size_t size, bytes_needed, i;
        uint8_t *ptr;

        /*
         * We may need to grow the tree to be able to fit an allocation of this
         * size. Try to grow the tree and stop here if we can't.
         */
        if (!lower_bucket_limit(bucket)) {
            return NULL;
        }

        /*
         * Try to pop a block off the free list for this bucket. If the free list
         * is empty, we're going to have to split a larger block instead.
         */
        ptr = (uint8_t *)list_pop(&buckets[bucket]);
        if (!ptr) {
            /*
             * If we're not at the root of the tree or it's impossible to grow the
             * tree any more, continue on to the next bucket.
             */
            if (bucket != bucket_limit || bucket == 0) {
                bucket--;
                continue;
            }

            /*
             * Otherwise, grow the tree one more level and then pop a block off the
             * free list again. Since we know the root of the tree is used (because
             * the free list was empty), this will add a parent above this node in
             * the SPLIT state and then add the new right child node to the free list
             * for this bucket. Popping the free list will give us this right child.
             */
            if (!lower_bucket_limit(bucket - 1)) {
                return NULL;
            }
            ptr = (uint8_t *)list_pop(&buckets[bucket]);
        }

        /*
         * Try to expand the address space first before going any further. If we
         * have run out of space, put this block back on the free list and fail.
         */
        size = (size_t)1 << (MAX_ALLOC_LOG2 - bucket);
        bytes_needed = bucket < original_bucket ? size / 2 + sizeof(list_t) : size;
        if (!update_max_ptr(ptr + bytes_needed)) {
            list_push(&buckets[bucket], (list_t *)ptr);
            return NULL;
        }

        /*
         * If we got a node off the free list, change the node from UNUSED to USED.
         * This involves flipping our parent's "is split" bit because that bit is
         * the exclusive-or of the UNUSED flags of both children, and our UNUSED
         * flag (which isn't ever stored explicitly) has just changed.
         *
         * Note that we shouldn't ever need to flip the "is split" bit of our
         * grandparent because we know our buddy is USED so it's impossible for our
         * grandparent to be UNUSED (if our buddy chunk was UNUSED, our parent
         * wouldn't ever have been split in the first place).
         */
        i = node_for_ptr(ptr, bucket);
        if (i != 0) {
            flip_parent_is_split(i);
        }

        /*
         * If the node we got is larger than we need, split it down to the correct
         * size and put the new unused child nodes on the free list in the
         * corresponding bucket. This is done by repeatedly moving to the left
         * child, splitting the parent, and then adding the right child to the free
         * list.
         */
        while (bucket < original_bucket) {
            i = i * 2 + 1;
            bucket++;
            flip_parent_is_split(i);
            list_push(&buckets[bucket], (list_t *)ptr_for_node(i + 1, bucket));
        }

        /*
         * Now that we have a memory address, write the block header (just the size
         * of the allocation) and return the address immediately after the header.
         */
        *(size_t *)ptr = request;
        return ptr + HEADER_SIZE;
    }

    return NULL;
}

void free(void *ptr) {
    size_t bucket, i;

    /*
     * Ignore any attempts to free a NULL pointer.
     */
    if (!ptr) {
        return;
    }

    /*
     * We were given the address returned by "malloc" so get back to the actual
     * address of the node by subtracting off the size of the block header. Then
     * look up the index of the node corresponding to this address.
     */
    ptr = (uint8_t *)ptr - HEADER_SIZE;
    bucket = bucket_for_request(*(size_t *)ptr + HEADER_SIZE);
    i = node_for_ptr((uint8_t *)ptr, bucket);

    /*
     * Traverse up to the root node, flipping USED blocks to UNUSED and merging
     * UNUSED buddies together into a single UNUSED parent.
     */
    while (i != 0) {
        /*
         * Change this node from UNUSED to USED. This involves flipping our
         * parent's "is split" bit because that bit is the exclusive-or of the
         * UNUSED flags of both children, and our UNUSED flag (which isn't ever
         * stored explicitly) has just changed.
         */
        flip_parent_is_split(i);

        /*
         * If the parent is now SPLIT, that means our buddy is USED, so don't merge
         * with it. Instead, stop the iteration here and add ourselves to the free
         * list for our bucket.
         *
         * Also stop here if we're at the current root node, even if that root node
         * is now UNUSED. Root nodes don't have a buddy so we can't merge with one.
         */
        if (parent_is_split(i) || bucket == bucket_limit) {
            break;
        }

        /*
         * If we get here, we know our buddy is UNUSED. In this case we should
         * merge with that buddy and continue traversing up to the root node. We
         * need to remove the buddy from its free list here but we don't need to
         * add the merged parent to its free list yet. That will be done once after
         * this loop is finished.
         */
        list_remove((list_t *)ptr_for_node(((i - 1) ^ 1) + 1, bucket));
        i = (i - 1) / 2;
        bucket--;
    }

    /*
     * Add ourselves to the free list for our bucket. We add to the back of the
     * list because "malloc" takes from the back of the list and we want a "free"
     * followed by a "malloc" of the same size to ideally use the same address
     * for better memory locality.
     */
    list_push(&buckets[bucket], (list_t *)ptr_for_node(i, bucket));
}
