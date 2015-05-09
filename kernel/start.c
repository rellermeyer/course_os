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
#include "hw_handlers.h"
#include "global_defs.h"
#include "argparse.h"
#include "interrupt.h"
#include "mmap.h"
#include "process.h"
#include "memory.h"
#include "drivers/uart.h"
#include "drivers/mmci.c"
#include "klibc.h"
#include "vm.h"
#include "include/open_table.h" //to initialize fs opentable
#include "mem_alloc.h"
#include "tests.h"
#include "drivers/timer.h"
#include "scheduler.h"

// Tests
#include "tests/test_priority_queue.h"
#include "tests/test_hash_map.h"
#include "tests/test_mem_alloc.h"
#include "tests/test_vm.h"
#include "tests/test_kthreads.h"
#include "tests/test_process.h"
#include "tests/test_sched_process.h"
#include "tests/test_user_mode.h"

#include "hashtable.h"
#include "streams.h"

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))

extern int process_global_init();

void uart_handler(void *null) {
	print_uart0("uart0!\n");
}

// This start is what u-boot calls. It's just a wrapper around setting up the
// virtual memory for the kernel.
void start(uint32_t *p_bootargs) {
	/*NOTICE: Do not use os_printf in start*/

	// Initialize the virtual memory
	vm_init();
	mmap(p_bootargs);
}

// This start is what starts the kernel. Note that virtual memory is enabled
// at this point (And running, also, in the kernel's VAS).
void start2(uint32_t *p_bootargs) {

	// Initialize
	// ----------
	// Setup all of the exception handlers... (hrm, interaction with VM?)

	init_vector_table();
	init_heap();
	q_create("printf");
	int qd = q_open("printf");
	q_subscribe(qd, printf_receiver, 0x0);
	sched_init();
	kfs_init(0, 0, 0);
	process_global_init();

	//Test: Streams tesing
	ht_test();
	q_test();

	//Test: UART0 mapped to the correct virtual address

	print_uart0("\nCourseOS!\n");

	// Test cases
	// ----------
	// run_vm_tests();
	// run_mem_alloc_tests();
	run_prq_tests();
	run_hmap_tests();
	run_kthr_tests();
	// run_arrl_tests();
	run_sched_prcs_tests();
	// run_prcs_tests();
	// run_fs_tests();
	// run_umode_tests();

	INFO("System ready!");

	while (1) {
		// wait
	}
}
