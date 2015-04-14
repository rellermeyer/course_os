#include "klibc.h"
#include "hashtable.h"

uint32_t hash_string(const char *str) {
	return 0;
}

struct ht *ht_alloc(int nentries) {
	struct ht *table = kmalloc(sizeof(struct ht));
	table->size = nentries;
	table->entries = kmalloc(sizeof(struct ht_entry)*nentries);
	return table;
}

struct ht_entry *ht_get_entry(struct ht *table, const char *key)
{
	uint32_t hash = hash_string(key);
	struct ht_entry *entry = &table->entries[hash%table->size];
	while (entry && entry->key.key && os_strcmp(entry->key.key, key)) {
		entry = entry->next_entry;
	}
	return entry;
}

int ht_add(struct ht *table, const char *key, void *value) {
	uint32_t hash = hash_string(key);
	struct ht_entry *entry = &table->entries[hash%table->size];
	while (1) {
		if (!os_strcmp(entry->key.key, key)) {
			// Fill this entry
			break;
		}
		if (entry->key.key == 0x0) {
			// Fill this entry
			break;
		} else if (entry->next_entry == 0x0) {
			entry->next_entry = kmalloc(sizeof(struct ht_entry));
			entry = entry->next_entry;
			// Fill this entry
			break;
		}
	}
	entry->value = value;
	return 0;
}

int ht_remove(struct ht *table, const char *key) {
	// Currently unimplemented
	return -1;
}

void *ht_get(struct ht *table, const char *key) {
	struct ht_entry *entry = ht_get_entry(table, key);
	if ((!entry) || !entry->key.key) {
		return 0x0;
	}
	return entry->value;
}
