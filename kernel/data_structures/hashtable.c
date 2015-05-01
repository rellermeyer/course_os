#include "klibc.h"
#include "hashtable.h"

// "str" must be null-terminated
uint32_t hash_string(const char *str) {
	uint32_t hash = 123456789;
	int i;
	for (i=0; str[i]; i++) {
		hash = hash*33 + str[i];
	}
	return hash;
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
		if (entry->key.hash == hash && !os_strcmp(entry->key.key, key)) {
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
		entry = entry->next_entry;
	}
	entry->value = value;
	entry->key.hash = hash;
	entry->key.key = kmalloc(os_strlen(key)+1);
	os_strcpy(entry->key.key, key);
	return 0;
}

int ht_remove(struct ht *table, const char *key) {
	uint32_t hash = hash_string(key);
	struct ht_entry *entry = &table->entries[hash%table->size];
	struct ht_entry *last = 0x0;
	while (1) {
		if ((!entry) || !entry->key.key) {
			return 0;
		}
		if (os_strcmp(entry->key.key, key)) {
			entry = entry->next_entry;
		} else {
			// We found it.
			kfree(entry->key.key);
			entry->key.key = 0x0;
			entry->value = 0x0;
			if (last) {
				last->next_entry = entry->next_entry;
			} else if (entry->next_entry) {
				os_memcpy((uint32_t*)entry, (uint32_t*)entry->next_entry, sizeof(struct ht_entry));
			}
			return 1;
		}
	}
	return -1;
}

void *ht_get(struct ht *table, const char *key) {
	struct ht_entry *entry = ht_get_entry(table, key);
	if ((!entry) || !entry->key.key) {
		return 0x0;
	}
	return entry->value;
}

void ht_test_hasher(const char *key) {
}

void ht_test() {

	// Let's test the string hashing function for a few strings
	ht_test_hasher("");
	ht_test_hasher("Lane");
	ht_test_hasher("Mark");
	ht_test_hasher("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNRSTUVWXYZ\r\n");

	// Now test the actual table...
	struct ht *table = ht_alloc(6);

	// Add a bunch of stuff (load factor = 1.5, chaining is tested)
	ht_add(table, "abcde", (void*)1);
	ht_add(table, "ABCDE", (void*)2);
	ht_add(table, "EDACSDAFG", (void*)3);
	ht_add(table, "aaaaaaaaaa", (void*)4);
	ht_add(table, "abcdej", (void*)5);
	ht_add(table, "abcdei", (void*)6);
	ht_add(table, "abcdeh", (void*)7);
	ht_add(table, "abcdeg", (void*)8);
	ht_add(table, "abcdef", (void*)9);

	// Retrieve some stuff...
	if ((int)ht_get(table, "abcdeh") == 7) {
	} else {
	}

	// Remove some stuff...
	ht_remove(table, "abcdeg");

	// Make sure it's gone...
	if ((int)ht_get(table, "abcdeg") == 0) {
	} else {
	}

}
