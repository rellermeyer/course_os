#include "tests.h"
#include "tests/test_mem_alloc.h"
#include "klibc.h"
#include "vm.h"

#define NUM_TESTS 1

/*
objects += .o files of testthen make blah blah blah of build targetof upper make file, will make kernel with tests
*/

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
    if (kmcheck()) {
        ERROR("Inconsistent memory to begin with...\n");
        return TEST_FAIL;
    }

    // Allocate some memory
    char *p = kmalloc(15);
    p = kmalloc(15);
    os_strcpy(p, "Hello!");

    DEBUG("%s\n", p);

    if (kmcheck()) {
        ERROR("allocate(15) failed.\n\n");
        return TEST_FAIL;
    }

    // Allocate a bunch of blocks of "random" size
    uint64_t rng = 1234567891;

    char *pntrs[256];
    uint32_t alloced = 0;
    int i;
    DEBUG("Starting out w/ %u bytes of heap\n", km_size());

    for (i = 0; i < 256; i++) {
        uint32_t size = gen_rand(&rng, 15);
        pntrs[i] = kmalloc(size);
        alloced += size;
        DEBUG("%u %u %u %u %d\n", i, km_size(), size, alloced,
                vm_count_free_frames());
    }

    // Test one of them
    pntrs[230][0] = 1;

    if(pntrs[230][0] != 1){
        ERROR("%d != 1\n", pntrs[230][0]);
        return TEST_FAIL;
    }

    if (kmcheck()) {
        ERROR("Memory is inconsistent :-(\n");
        return TEST_FAIL;
    }

    // Free all the pntrs
    for (i = 0; i < 256; i++) {
        if (pntrs[i]) {
            kfree(pntrs[i]);
        }
    }

    kfree(p);

    if (kmcheck()) {
        ERROR("Memory is inconsistent :-(\n");
	return TEST_FAIL;
    }

    DEBUG("heap_size = %d bytes\n", km_size());

    return TEST_OK;
}

//function running tests
void run_mem_alloc_tests() {
    Test *tests[NUM_TESTS];
    Test a; // We can't rely on kmalloc for a kmalloc test.
    a.test_name = "test_mem_alloc_1";
    a.testptr = &test_mem_alloc_1;
    tests[0] = &a;
    //tests[0] = create_test("test_mem_alloc_1", &test_mem_alloc_1);
    os_printf("asdf\n");
    run_tests(tests, NUM_TESTS);
}
