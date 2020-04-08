#include <HashMap.h>
#include <stdlib.h>
#include <test.h>


size_t int_hash_fn(const void * key, void * ctx) {
    return (size_t) * (int *)key;
}

bool int_compare_fn(const void * key1, const void * key2, void * ctx) {
    return *(int *)key1 == *(int *)key2;
}

void fakeFree(void * fake) {
    UNUSED(fake);
}

void simpleFree(void * val) {
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

    int * key = (int *)kmalloc(sizeof(int));
    int * value = (int *)kmalloc(sizeof(int));
    *key = 42;
    *value = 69;

    hashmap__add(hm, key, value);

    hashmap__free(hm);

    // Test framework will assert that we won't leak.
})

TEST_CREATE(test_find_hm, {
    HashMap * hm = hashmap__new(int_hash_fn, int_compare_fn, fakeFree, fakeFree, NULL);

    int key = 5;
    int data = 42;

    hashmap__add(hm, &key, &data);

    ASSERT_EQ(hashmap__size(hm), 1);

    int * ptr = NULL;
    void ** dp = (void **)&ptr;

    ASSERT(hashmap__find(hm, &key, dp));
    ASSERT_EQ(&data, *dp);
    ASSERT_EQ(data, *ptr);

    hashmap__free(hm);
})

TEST_CREATE(test_set_hm, {
    HashMap * hm = hashmap__new(int_hash_fn, int_compare_fn, fakeFree, fakeFree, NULL);

    int key = 5;
    int data = 42;

    // add 42
    hashmap__add(hm, &key, &data);

    ASSERT_EQ(hashmap__size(hm), 1);

    int * ptr = NULL;
    void ** dp = (void **)&ptr;

    ASSERT(hashmap__find(hm, &key, dp));
    ASSERT_EQ(&data, *dp);
    ASSERT_EQ(data, *ptr);

    // set 42 to 69
    int new_data = 69;
    hashmap__set(hm, &key, &new_data, NULL, NULL);

    ASSERT(hashmap__find(hm, &key, dp));
    ASSERT_EQ(&new_data, *dp);
    ASSERT_EQ(new_data, *ptr);

    // set (144,196) which doesn't exist yet
    int extra_data = 196;
    int extra_key = 144;
    hashmap__set(hm, &extra_key, &extra_data, NULL, NULL);

    ASSERT(hashmap__find(hm, &extra_key, dp));
    ASSERT_EQ(&extra_data, *dp);
    ASSERT_EQ(extra_data, *ptr);

    hashmap__free(hm);
})


TEST_CREATE(test_update_hm, {
    HashMap * hm = hashmap__new(int_hash_fn, int_compare_fn, fakeFree, fakeFree, NULL);

    int key = 5;
    int data = 42;

    // add 42
    hashmap__add(hm, &key, &data);

    ASSERT_EQ(hashmap__size(hm), 1);

    int * ptr = NULL;
    void ** dp = (void **)&ptr;

    ASSERT(hashmap__find(hm, &key, dp));
    ASSERT_EQ(&data, *dp);
    ASSERT_EQ(data, *ptr);

    // set 42 to 69
    int new_data = 69;
    hashmap__update(hm, &key, &new_data, NULL, NULL);

    ASSERT(hashmap__find(hm, &key, dp));
    ASSERT_EQ(&new_data, *dp);
    ASSERT_EQ(new_data, *ptr);

    // set (144,196) which doesn't exist
    int extra_data = 196;
    int extra_key = 144;
    ASSERT_EQ(hashmap__update(hm, &extra_key, &extra_data, NULL, NULL), -1);

    hashmap__free(hm);
})
