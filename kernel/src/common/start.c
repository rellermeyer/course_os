/*
 *  A bit of background:
 *  - The ARM architecture has 7 modes of operation:
 *      + USR - user mode
 *      + FIQ - processing "fast" interrupts
 *      + IRQ - processing "normal" interrupts
 *      + SVC - proctected mode for OS
 *      + UND - processing an undefined instruction exception
 *      + SYS - also protecteed mode for OS --if anyone wants to clarify, feel free--
 *
 *  - These modes can be entered or exited by modifying the CPSR (status register), first 5 bits
 *	+ 0b10000 = user mode
 *	+ 0b10001 = FIQ (fast interrupt) mode
 *	+ 0b10010 = IRQ (normal interrupt) mode
 *	+ 0b10011 = SVC (supervisor, or, OS) mode
 *	(others...)
 */

#include <stdint.h>
#include <interrupt.h>
#include <mmap.h>
#include <process.h>
#include <klibc.h>
#include <vm.h>
#include <scheduler.h>
#include <mem_alloc.h>
#include <test.h>
#include <hardwareinfo.h>
#include <chipset.h>
#include <vm2.h>

// This start is what u-boot calls. It's just a wrapper around setting up the
// virtual memory for the kernel.
void start(uint32_t *p_bootargs) {
    prepare_pagetable();
    vm2_prepare();

    // Before this point, all code has to be hardware independent.
    // After this point, code can request the hardware info struct to find out what
    // Code should be ran.
    init_hardwareinfo();

    // Initialize the chipset and enable uart
    init_chipset();

    print_hardwareinfo();

    kprintf("Enabling MMU...\n");
    vm_init();
    kprintf("Initialized VM datastructures.\n");

    vm2_start();

    // Paging and virtual memory is initialized. This code jumps us to start2.
    mmap(p_bootargs);
}

// This start is what starts the kernel. Note that virtual memory is enabled
// at this point (And running, also, in the kernel's VAS).
void start2(uint32_t *p_bootargs) {
    kprintf("start address: 0x%x\n", start2);

    // Set up the exception handlers.
    init_vector_table();

    // After this point kmalloc and kfree can be used for dynamic memory management.
    init_heap();

    splash();


    // Turn on interrupts
    enable_interrupt(BOTH);

    // Call the chipset again to do post-interrupt-enable initialization
    chipset.late_init();

    process_init();

    // FIXME: temporary
    sched_init();

    kprintf("bootargs: 0x%x\n", p_bootargs);


#ifndef ENABLE_TESTS
//    argparse_process(p_bootargs);
//
// TODO: Start init process
#else
    test_main();
    // If we return, the tests failed.
    SemihostingCall(OSSpecific);
#endif
    kprintf("done parsing atag list\n");

    //init_kheap(31 * 0x100000);
    //init_uheap(0x100000);

    //initialize pcb table and PID
    /* init_all_processes(); */
    //print_process_state(0);
    //run_process_tests();
    //print_PID();
    // init_q();
    //common();

    // TODO:
    //  * Mount vfs
    //  * Load initramfs into tmpfs
    //  * execute userland init program

    asm volatile("cpsie i");

    kprintf("End of start method.\n");
    SLEEP;
}
