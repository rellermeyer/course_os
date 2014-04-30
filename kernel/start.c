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

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))
void uart_handler(void *null) {
	print_uart0("uart0!\n");
}


void start(void *p_bootargs) {

	init_vector_table();
	asm volatile("SWI 7");

	
	/* Simulating/Testing Interrupt Routines with UART */
	
	/* THIS PART IS  DONE IN THE DRIVER */
	// Step 1: arm interrupts on device
	/* enable RXIM interrupt */
 	UART0_IMSC = 1<<4;
	
	// Step 2: Create handler
	interrupt_handler_t uartHandler;
	uartHandler.handler = uart_handler;
	
	// Step 3: register the handler with interrupts 
	// (12 = interrupt mapping for UART handler -- see interrupt.h)
	register_interrupt_handler(12, &uartHandler);
	// now we go off into interrupt land ...
	// ... ok we're back from the interrupt
	
	// Step 4: disarm the interrupt on the device
	
}
