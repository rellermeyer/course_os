/*
 *
 * Hash Map (source found online)
 *
 * contact: Mathew Kurian <bluejamesbond@gmail.com>
 *
 */

#include "../include/hash_map.h"
#include "klibc.h"

/* this should be prime */
#define TABLE_STARTSIZE 1021
#define ACTIVE 1

static unsigned long __hmap_is_prime(unsigned long val) {
    int i, p, exp, a;

    for (i = 9; i--;) {
        a = (rand() % (val - 4)) + 2;
        p = 1;
        exp = val - 1;
        while (exp) {
            if (exp & 1) {
                p = (p * a) % val;
            }

            a = (a * a) % val;
            exp >>= 1;
        }

        if (p != 1) {
            return 0;
        }
    }

    return 1;
}

static int __hmap_find_prime_greater_than(int val) {
    if (val & 1) {
        val += 2;
    } else {
        val++;
    }

    while (!__hmap_is_prime(val)) {
        val += 2;
    }

    return val;
}

static void __hmap_rehash(hmap_handle* hmap) {
    long size = hmap->size;
    hmap_entry* table = hmap->table;

    hmap->size = __hmap_find_prime_greater_than(size << 1);
    hmap->table = (hmap_entry*) kmalloc(sizeof(hmap_entry) * hmap->size);
    os_memset(hmap->table, 0, sizeof(hmap_entry) * hmap->size);
    hmap->count = 0;

    while (--size >= 0) {
        if (table[size].flags == ACTIVE) {
            hmap_put(hmap, table[size].key, table[size].data);
        }
    }

    kfree(table);
}

/**
 * Creates a hash map with default size
 * Params:
 * 		None
 * Return: hmap_handle*
 */
hmap_handle* hmap_create(){
    return hmap_create_fixed(TABLE_STARTSIZE);
};

/**
 * Creates a hash map with desired size
 * Params:
 * 		size of the hash map to be created
 * Return: hmap_handle*
 */
hmap_handle* hmap_create_fixed(int startsize) {
    hmap_handle* hmap = (hmap_handle*) kmalloc(sizeof(hmap_handle));

    if (!startsize) {
        startsize = TABLE_STARTSIZE;
    } else {
        startsize = __hmap_find_prime_greater_than(startsize - 2);
    }

    hmap->table = (hmap_entry*) kmalloc(sizeof(hmap_entry) * startsize);

    os_memset(hmap->table, 0, startsize);

    hmap->size = startsize;
    hmap->count = 0;

    return hmap;
}

/**
 * insert data to hash map
 * Params:
 * 		hmap_handle*: hash map to which element will be added to
 * 		key: the key by which data will be searched
 * 		void*: pointer to element to be added
 * Return: void
 *
 */
void hmap_put(hmap_handle* hmap, unsigned long key, const void* data) {
    long index, i, step;

    if (hmap->size <= hmap->count)
        __hmap_rehash(hmap);

    do {
        index = key % hmap->size;
        step = (key % (hmap->size - 2)) + 1;

        for (i = 0; i < hmap->size; i++) {
            if (hmap->table[index].flags & ACTIVE) {
                if (hmap->table[index].key == key) {
                    hmap->table[index].data = (void*) data;
                    return;
                }
            } else {
                hmap->table[index].flags |= ACTIVE;
                hmap->table[index].data = (void*) data;
                hmap->table[index].key = key;
                ++hmap->count;
                return;
            }

            index = (index + step) % hmap->size;
        }

        /* it should not be possible that we EVER come this far, but unfortunately
         not every generated prime number is prime (Carmichael numbers...) */
        __hmap_rehash(hmap);
    } while (1);
}

/**
 * removes the given element from hash map
 * Params:
 * 		hmap_handle*: hash map from which element will be removed
 * 		unsigned long: key of the element to be removed
 * Return: pointer to element to be removed
 *
 */
void* hmap_remove(hmap_handle* hmap, unsigned long key) {
    long index, i, step;

    index = key % hmap->size;
    step = (key % (hmap->size - 2)) + 1;

    for (i = 0; i < hmap->size; i++) {
        if (hmap->table[index].data) {
            if (hmap->table[index].key == key) {
                if (hmap->table[index].flags & ACTIVE) {
                    hmap->table[index].flags &= ~ACTIVE;
                    --hmap->count;
                    return hmap->table[index].data;
                } else {
                    /* in, but not active (i.e. deleted) */
                    return 0;
                }
            }
        } else {
            /* found an empty place (can't be in) */
            return 0;
        }

        index = (index + step) % hmap->size;
    }
    /* everything searched through, but not in */
    return 0;
}

/**
 * returns an element from hash map by key
 * Params:
 * 		hmap_handle*: hash map from which element will be returned
 * 		unsigned long: key of the element to be returned
 * Return: pointer to element to be returned
 *
 */
void* hmap_get(hmap_handle* hmap, unsigned long key) {
    if (hmap->count) {
        long index, i, step;
        index = key % hmap->size;
        step = (key % (hmap->size - 2)) + 1;

        for (i = 0; i < hmap->size; i++) {
            if (hmap->table[index].key == key) {
                if (hmap->table[index].flags & ACTIVE) {
                    return hmap->table[index].data;
                }
                break;
            } else if (!hmap->table[index].data) {
                break;
            }

            index = (index + step) % hmap->size;
        }
    }

    return 0;
}

/**
 * returns the number of elements in hash map
 * Params:
 * 		hmap_handle*: hash map whose element count needs to be returned
 * Return: the number of elements in hash map
 *
 */
long hmap_count(hmap_handle* hash) {
    return hash->count;
}

/**
 * frees given hash map
 * Params:
 * 		hmap_handle*: hash map to be freed
 * Return: void
 *
 */
void hmap_free(hmap_handle* hash) {
    kfree(hash->table);
    kfree(hash);
}

