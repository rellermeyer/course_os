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
#include "pmap.h"
#include "vmlayout.h"

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))
void uart_handler(void *null) {
	print_uart0("uart0!\n");
}

void start(void *p_bootargs) {

	// we're not using Virtual memory here
	//	mmap();  
	init_vector_table();
	asm volatile("SWI 7");

	asm volatile (".include \"stacks.s\"");

	/* NOTHING EXECUTED BEYOND THIS POINT
	 *
	 *
	 * Anything that needs to be setup right after
	 * booting the kernel should go before mmap()
	 *
	 * Any setup, heap allocation or stack allocation
	 * goes in main
	 *
	 */
}
