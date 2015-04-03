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
#include "klibc.h"

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))

extern int init_all_processes();

void uart_handler(void *null)
{
	print_uart0("uart0!\n");
}

void start(uint32_t *p_bootargs)
{
	print_uart0("\nCourseOS!\n");

	argparse_process(p_bootargs);

	print_uart0("done parsing atag list\n");

	init_vector_table();
        asm volatile("swi 1");
	//Test: UART0 mapped to the correct virtual address
        /* print_uart0("MMU enabled\n"); */
	//init_kheap(31 * 0x100000);
	//init_uheap(0x100000);

	//initialize pcb table and PID
	/* init_all_processes(); */
	//print_process_state(0);

	//run_process_tests();

	//print_PID();
	// init_q();

	//main();
	//asm volatile("wfi");
}
