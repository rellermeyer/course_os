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
// #include "scheduler.h"

// Tests
#include "tests/test_priority_queue.h"
#include "tests/test_hash_map.h"
#include "tests/test_mem_alloc.h"
#include "tests/test_vm.h"

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))

extern int init_all_processes();
extern int vm_count_free_frames();

void uart_handler(void *null)
{
	print_uart0("uart0!\n");
}

// This start is what u-boot calls. It's just a wrapper around setting up the
// virtual memory for the kernel.
void start(uint32_t *p_bootargs)
{
	// Initialize the virtual memory
	print_uart0("Enabling MMU...\n");
	/*print_uart0("p_bootargs: ");
	print_uart0((char*)p_bootargs);
	print_uart0("\n");*/
	os_printf("%X\n",*p_bootargs);
	vm_init();
	os_printf("Initialized VM datastructures.\n");
	mmap(p_bootargs);
}

void vm_test_early();

// This start is what starts the kernel. Note that virtual memory is enabled
// at this point (And running, also, in the kernel's VAS).
void start2(uint32_t *p_bootargs)
{


	// Setup all of the exception handlers... (hrm, interaction with VM?)
	init_vector_table();

	//vm_test_early();

	// Setup kmalloc...
	init_heap();
	//_schedule_register_timer_irq();
        timer_test();

	//Test: UART0 mapped to the correct virtual address
	print_uart0("MMU enabled\n");
	//asm volatile("swi 1");
	//while (1);

	print_uart0("\nCourseOS!\n");
	//p_bootargs = (uint32_t*)0x100;
	INFO("Bootargs: %X\n",*p_bootargs);
	/*print_uart0((char*)p_bootargs);
	  print_uart0("\n");*/

	// Test stuff...
	/*int *p = (int*)0xFFFFFFF0;
	p[0] = 1;
	os_printf("0x%x == 1?\n", p[0]);*/
	//_schedule_register_timer_irq();
	///timer_test();
/*	run_vm_tests();
	INFO("There are %d free frames.\n", vm_count_free_frames());
	//run_mem_alloc_tests();
	INFO("There are %d free frames.\n", vm_count_free_frames());
	run_prq_tests();
	run_hmap_tests();
*/

	//run_fs_tests();

	// int fd = kopen("/hello", 'r');
	// os_printf("fd: %d\n", fd);
	// kclose(fd);

	//while(1);

	//asm volatile("swi 1");

	/*
	4-15-15: 	#Prakash: 	What happens if we let the program load here?
							Let's make argparse_process() do its thing

				Note: As of 4-15-15 this fails horribly with hello.o not being
				recognized as an ELF file and DATA ABORT HANDLER being syscalled			   
	*/

	//assert(1==2 && "Test assert please ignore");

	init_all_processes();
//	argparse_process(p_bootargs);
	

	print_uart0("done parsing atag list\n");

	//init_kheap(31 * 0x100000);
	//init_uheap(0x100000);

	//initialize pcb table and PID
	/* init_all_processes(); */
	//print_process_state(0);

	//run_process_tests();
//	timer_test();
	//print_PID();
	// init_q();

	//main();
	//asm volatile("wfi");
	while (1);
}
