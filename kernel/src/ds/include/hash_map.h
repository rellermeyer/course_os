/*
 * hash_map.h
 *
 *  Created on: Apr 18, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_HASH_MAP_H_
#define KERNEL_INCLUDE_HASH_MAP_H_

typedef struct {
    void* data;
    int flags;
    long key;
} hmap_entry;

typedef struct hmap_handle {
    hmap_entry* table;
    long size, count;
} hmap_handle;

hmap_handle* hmap_create();
hmap_handle* hmap_create_fixed(int startsize);
void* hmap_get(hmap_handle* hmap, unsigned long key);
void hmap_put(hmap_handle* hmap, unsigned long key, const void* data);
void* hmap_remove(hmap_handle* hmap, unsigned long key);
long hmap_count(hmap_handle* hash);
void hmap_free(hmap_handle* hash);

#endif /* KERNEL_INCLUDE_HASH_MAP_H_ */
