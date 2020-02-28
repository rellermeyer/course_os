#include <test.h>
#include "klibc.h"
#include "memory.h"
#include "vm.h"

// TODO: This test has many LOG calls, we should try and looking at improving the logging capabilities
TEST_CREATE(test_vm_1, {
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

        // Do we still have the stack?
        // FIXME Update constant as necessary
        #define STACK_ADDR 0xF020000C
        // Invalid stack
        ASSERT_EQ((unsigned int) vas1, STACK_ADDR);

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
        #define P3_BASE 0x24000000
        int retval = vm_allocate_page(vas3, (void*) P3_BASE, VM_PERM_PRIVILEGED_RW);

        // vm_allocate_page fails
        ASSERT(!retval);

        int *p = (int*) 0xFFFFFFF0;
        p[0] = 1;

        ASSERT_EQ(p[0], 1);

        // Oh man! We should be able to write to there!
        p = (int*) P3_BASE;
        p[0] = 1;

        LOG("%x %x\n", &p, p);

        ASSERT_EQ(p[0], 1);

        // Test shared memory...
        LOG("Testing shared memory...\n");
        int *p_3 = (int*) P3_BASE;//0x24000000;
        int *p_1 = (int*) 0x31000000;
        retval = vm_map_shared_memory(vas1, p_1, vas3, p_3, VM_PERM_PRIVILEGED_RW);
        LOG("map_shared_memory returned %d\n", retval);

        p_3[0] = 321;
        vm_enable_vas(vas1);
        ASSERT_EQ(p_1[0], 321);
        p_1[1] = 456;

        vm_enable_vas(vas3);
        ASSERT_EQ(p_3[1], 456);

        // Test allocating many frames... (this was commented but seems to work just fine)
       *p += BLOCK_SIZE;
        while (!vm_allocate_page(vas3, (void*) p, 0)) {
            //LOG("Allocated memory...\n");
            p += BLOCK_SIZE;
        }

        p -= BLOCK_SIZE;
        kprintf("Highest frame allocated: 0x%X\n", p);

        while ((unsigned int) p > P3_BASE) {
            //LOG("Freed memory...\n");
            vm_free_page(vas3, p);
            p -= BLOCK_SIZE;
        }

        // FIXME: This part of the test crashes the os, we should catch that in some way.
        // Test the data abort...
        //WARN("You should see a data abort...\n");
        //int i = p[-1];
        //LOG("%d\n", i);

        // Free the page!
        LOG("Freeing page at %X\n", p);
        vm_free_page(vas3, p);

        // Clean up & switch back to the kernel's VAS before we return.
        vm_enable_vas((struct vas*) KERNEL_VAS);
})
