#include <test.h>
#include <HashMap.h>
#include <stdlib.h>


size_t int_hash_fn(const void *key, void *ctx) {
    return (size_t) *(int *)key;
}

bool int_compare_fn(const void *key1, const void *key2, void *ctx) {
    return *(int *)key1 == *(int *)key2;
}

void fakeFree(void * fake){
    UNUSED(fake);
}

void simpleFree(void * val){
    kfree(val);
}

TEST_CREATE(test_create_hm, {
    HashMap * hm = hashmap__new(int_hash_fn, int_compare_fn, fakeFree, fakeFree, NULL);

    int key = 5;
    int data = 42;

    hashmap__add(hm, &key, &data);

    ASSERT(hashmap__find(hm, &key, NULL));

    hashmap__free(hm);
})



TEST_CREATE(test_free_entries_hm, {
    HashMap * hm = hashmap__new(int_hash_fn, int_compare_fn, simpleFree, simpleFree, NULL);

    int * key = (int *) kmalloc(sizeof(int));
    int * value = (int *) kmalloc(sizeof(int));
    *key = 42;
    *value = 69;

    hashmap__add(hm, key, value);

    hashmap__free(hm);

    // Test framework will assert that we won't leak.
})
