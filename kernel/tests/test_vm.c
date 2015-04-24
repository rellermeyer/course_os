#include "tests.h"
#include "klibc.h"
#include "memory.h"
#include "vm.h"

#define NUM_TESTS 1

int test_vm_1() {

    struct vas *vas1 = vm_new_vas();
    LOG("Got new vas at 0x%X\n", vas1);

    // We're part of the kernel, which is already mapped into vas1.
    // But our stack isn't, so let's add that mapping.
    unsigned int mystack = (unsigned int) &vas1;
    mystack &= 0xFFF00000; // Round down to nearest MB
    LOG("Stack addr: 0x%X\n", mystack);
    LOG("Created page table w/ 0xFFF00000's entry = 0x%X\n",
            vas1->l1_pagetable[V_KDSBASE>>20]);

    vm_enable_vas(vas1);

    // Can we still print?
    INFO("Hey, I'm printing!\n");

    // Do we still have the stack?
    // FIXME Update constant as necessary
#define STACK_ADDR 0xF020000C
    if (((int) vas1) != STACK_ADDR) {
        ERROR("Invalid stack");
        return TEST_FAIL;
    }

    struct vas *vas2 = (struct vas*) V_L1PTBASE;
    INFO("%X (%X)\n", vas2, &vas2);
    INFO("%X\n", *((unsigned int* ) vas2));
    INFO("%X\n", vas2->l1_pagetable);
    INFO("Entry: %x\n",
            vas1->l1_pagetable[(unsigned int ) vas2->l1_pagetable >> 20]);
    INFO("%X\n", vas2->l1_pagetable[0]);
    INFO("(deref: entry at 0x200000: 0x%X)\n",
            vas2->l1_pagetable[0x200000 >> 20]);

    // Test making a thing in this thing
    struct vas *vas3 = vm_new_vas();
    vm_enable_vas(vas3);
    INFO("%X and %X and %X\n", vas1, vas2, vas3);

    // Test allocating frames...
    int retval = vm_allocate_page(vas3, (void*) 0x25000000,
    VM_PERM_PRIVILEGED_RW);

    if (retval) {
        ERROR("ERROR: vm_allocate_page returned %x\n", retval);
        return TEST_FAIL;
    }

    int *p = (int*) 0xFFFFFFF0;
    p[0] = 1;

    if (p[0] != 1) {
        ERROR("0x%x == 1?\n", p[0]);
        return TEST_FAIL;
    }

    // Oh man! We should be able to write to there!
    p = (int*) 0x25000000;
    p[0] = 1;

    LOG("%x %x\n", &p, p);

    if (p[0] != 1) {
        ERROR("0x%x == 1?\n", p[0]);
        return TEST_FAIL;
    }

    // Test allocating many frames...
    p += BLOCK_SIZE;
    while (!vm_allocate_page(vas3, (void*) p, 0)) {
        p += BLOCK_SIZE;
    }

    p -= BLOCK_SIZE;
    LOG("Highest frame allocated: 0x%X\n", p);

    while ((unsigned int) p > 0x25000000) {
        vm_free_page(vas3, p);
        p -= BLOCK_SIZE;
    }

    // Test the data abort...
    WARN("You should see a data abort...\n");
    int i = p[-1];
    LOG("%d\n", i);

    // Free the page!
    vm_free_page(vas3, p);

    // Clean up & switch back to the kernel's VAS before we return.
    vm_enable_vas((struct vas*) V_L1PTBASE);

    return TEST_OK;
}

void vm_test_early() {
	os_printf("Test code for VM (early).\n");

#if 0
	// Test 4KB pages
	os_printf("0x%X\n", ((unsigned int *)(V_L1PTBASE + PAGE_TABLE_SIZE))[(PMAPBASE+0x100000)>>20]);
	os_printf("entry at the address: 0x%X\n", ((unsigned int *)(V_L1PTBASE + PAGE_TABLE_SIZE))[(PMAPBASE+0x100000)>>20]);
	unsigned int *p2 = (unsigned int*)(PMAPBASE+0x100000);
	os_printf("0x%X\n",p2);
	p2[1]++;
	p2[1023]++;
	os_printf("Should not have seen a page fault, should see one now.\n");
	p2[1024]++;

	// Hey, let's check the access bit now.
	p2 = ((unsigned int *)(V_L1PTBASE + PAGE_TABLE_SIZE));
	os_printf("Entry is the address: 0x%X\n", ((unsigned int *)(V_L1PTBASE + PAGE_TABLE_SIZE))[(PMAPBASE+0x100000)>>20]);
#endif

	os_printf("Leaving early test code for VM.\n");
	//while (1);
}

void run_vm_tests() {
    Test *tests[NUM_TESTS];
    tests[0] = create_test("test_vm_1", &test_vm_1);
    run_tests(tests, NUM_TESTS);
}
