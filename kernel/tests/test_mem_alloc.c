#include "../include/tests.h"
#include "../include/tests/test_mem_alloc.h"
#include "klibc.h"
#include "vm.h"

#define NUM_TESTS 1
#define VERBOSE

int vm_count_free_frames();

//nbits is 0-22
uint32_t gen_rand(uint64_t *state, int nbits) {
    *state = *state * 41 + (*state >> 5);
    uint32_t mask = ((1 << nbits) - 1) << 10;
    uint32_t v = (*state & mask);
    return v >> 10;
}

//Tests the create function
int test_mem_alloc_1() {
    if (!kmcheck()) {
        os_printf("Inconsistent memory to begin with...\n");
        return TEST_FAIL;
    }

    // Allocate some memory
    char *p = kmalloc(15);
    p = kmalloc(15);
    os_strcpy(p, "Hello!");
    os_printf("%s\n", p);

    if (!kmcheck()) {
        os_printf("allocate(15) failed.\n\n");
        return TEST_FAIL;
    }

    // Allocate a bunch of blocks of "random" size
    uint64_t rng = 1234567891;

    char *pntrs[256];
    uint32_t alloced = 0;
    int i;
    os_printf("Starting out w/ %u bytes of heap\n", km_size());

    for (i = 0; i < 256; i++) {
        uint32_t size = gen_rand(&rng, 15);
        pntrs[i] = kmalloc(size);
        alloced += size;
#ifdef VERBOSE
        os_printf("%u %u %u %u %d\n", i, km_size(), size, alloced,
                vm_count_free_frames());
#endif
    }

    // Test one of them
    pntrs[230][0] = 1;
    os_printf("%d == 1?\n", pntrs[230][0]);

    if (!kmcheck()) {
        os_printf("Memory is inconsistent :-(\n");
        return TEST_FAIL;
    } else {
        os_printf("Memory is consistent :-)\n");
    }

    // Free all the pntrs
    for (i = 0; i < 256; i++) {
        if (pntrs[i]) {
            kfree(pntrs[i]);
        }
    }

    kfree(p);

    if (!kmcheck()) {
        os_printf("Memory is inconsistent :-(\n");
    } else {
        os_printf("Memory is consistent :-)\n");
    }

    os_printf("heap_size = %d bytes\n", km_size());

    return TEST_OK;
}

//function running tests
void run_mem_alloc_tests() {
    Test *tests[NUM_TESTS];
    tests[0] = create_test("test_mem_alloc_1", &test_mem_alloc_1);

    run_tests(tests, NUM_TESTS);
}

