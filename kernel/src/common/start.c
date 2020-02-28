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
#include <hw_handlers.h>
#include <argparse.h>
#include <interrupt.h>
#include <mmap.h>
#include <process.h>
#include <uart.h>
#include <klibc.h>
#include <vm.h>
#include <timer.h>
#include <scheduler.h>
#include <mem_alloc.h>
#include <test.h>

// The file system has currently been *disabled* due to bugs
//#include "fs/open_table.h" //to initialize fs opentable
//#include "tests.h"


// Tests
//#include "../../tests/test_klibc.h"
//#include "tests/test_hash_map.h"
//#include "tests/test_mem_alloc.h"
//#include "tests/test_vm.h"
//#include "tests/test_priority_queue.h"

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))

void uart_handler(void *null) {
    print_uart0("uart0!\n");
}

// This start is what u-boot calls. It's just a wrapper around setting up the
// virtual memory for the kernel.
void start(uint32_t *p_bootargs) {
    // Initialize the virtual memory
    print_uart0("Enabling MMU...\n");
    vm_init();
    kprintf("Initialized VM datastructures.\n");
    mmap(p_bootargs);
}



// This start is what starts the kernel. Note that virtual memory is enabled
// at this point (And running, also, in the kernel's VAS).
void start2(uint32_t *p_bootargs) {
    // Setup all of the exception handlers... (hrm, interaction with VM?)
    init_vector_table();

    // Setup kmalloc...
    init_heap();

    //print_uart0("\nCourseOS!\n");

    splash();

    // Test stuff...
    /*int *p = (int*)0xFFFFFFF0;
     p[0] = 1;
     os_printf("0x%x == 1?\n", p[0]);*/

    //run_vm_tests();
    //INFO("There are %d free frames.\n", vm_count_free_frames());
    //run_mem_alloc_tests();
    //INFO("There are %d free frames.\n", vm_count_free_frames());
    //run_prq_tests();
    //run_hmap_tests();

    // The file system has currently been *disabled* due to bugs
    //	kfs_init(0, 0, 0);


    /*
     4-15-15: 	#Prakash: 	What happens if we let the program load here?
     Let's make argparse_process() do its thing

     Note: As of 4-15-15 this fails horribly with hello.o not being
     recognized as an ELF file and DATA ABORT HANDLER being syscalled
     */

    // enable interrupt handling
    enable_interrupts();

    // initialize the timers
    initialize_timers();

    process_init();

    sched_init();
    // FIXME: temporary
    kprintf("Programming the timer interrupt\n");
    start_timer_interrupts(0, 10);

#ifndef ENABLE_TESTS
    argparse_process(p_bootargs);
#else
    test_main();
    // If we return, the tests failed.
    shutdown_qemu_nonzero();
#endif

    print_uart0("done parsing atag list\n");

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

    SLEEP;
}
