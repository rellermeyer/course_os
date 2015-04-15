#ifndef HASHTABLE_H
#define HASHTABLE_H

#include<stdint.h>

struct ht_key {
	uint32_t hash;
	char *key;
};

// Note: keys are strings
struct ht_entry {
	struct ht_key key;
	void *value;
	struct ht_entry *next_entry;
};

struct ht {
	int size;
	struct ht_entry *entries;
};

struct ht *ht_alloc(int nentries);

int ht_add(struct ht *table, const char *key, void *value);
int ht_remove(struct ht *table, const char *key);
void *ht_get(struct ht *table, const char *key);
void ht_test();

#endif
