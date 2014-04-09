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
// #include "include/interrupt.h"
// #include "include/hw_handlers.h"

void start(void *p_bootargs) {
   print_uart0("CourseOS!\n");

	/* we boot into SVC mode with FIQ and IRQ masked/disabled */
	/* TODO: intialize the vector table, stack space, etc. */
	asm volatile(
		""
	);

	int cpsr = get_proc_status();
	print_word_bits(&cpsr);

	md((uint32_t *)0x00);
	
	// testing
	irq_handler();
}
