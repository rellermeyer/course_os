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
#include <klibc.h>
#include <mem_alloc.h>
#include <test.h>
#include <hardwareinfo.h>
#include <chipset.h>
#include <vm2.h>

// This start is what u-boot calls. It's just a wrapper around setting up the
// virtual memory for the kernel.
void start(uint32_t *p_bootargs) {

    // Before this point, all code has to be hardware independent.
    // After this point, code can request the hardware info struct to find out what
    // Code should be ran.
    init_hardwareinfo();

    // Initialize the chipset and enable uart
    init_chipset();

    INFO("Started chipset specific handlers");

    // just cosmetic (and for debugging)
    print_hardwareinfo();
    detect_boardtype();

    // start proper virtual and physical memory management.
    // Even though we already enabled the mmu in startup.s to
    // create a higher half kernel. The pagetable created there
    // was temporary and has to be replaced here.
    INFO("Initializing the physical and virtual memory managers.");
    vm2_start();

    INFO("Setting up interrupt vector tables");
    // Set up the exception handlers.
    init_vector_table();

    INFO("Setting up heap");
    // After this point kmalloc and kfree can be used for dynamic memory management.
    init_heap();


    splash();


    // Turn on interrupts
    enable_interrupt(BOTH);

    // Call the chipset again to do post-interrupt-enable initialization
    chipset.late_init();


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
