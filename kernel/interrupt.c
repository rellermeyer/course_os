/*
 *
 * Interrupts  
 *
 */
#include "include/interrupt.h"


interrupt_t IRQ = IRQ_MASK;
interrupt_t FIQ = FIQ_MASK;
interrupt_t ALL = ALL_INTERRUPT_MASK;

// In a system with an interrupt controller, software is required to:
// determine from the interrupt controller which interrupt source is requesting service
// determine where the service routine for that interrupt source is loaded
// mask or clear that interrupt source, before re-enabling processor interrupts to permit another interrupt to be taken.

// Interrupts must be enabled in three places:
// (1) the core (CPSR)
// (2) the VIC (interrupt controller)
// (3) (sometimes) in the device (this should be done in the device driver)

void handle_interrupt(int interrupt_vector){
 	// branch to interrupt routine and handle
	print_uart0("handling interrupt\n");
}


/* enable IRQ and/or FIQ */
void enable_interrupt(interrupt_t mask) {
	switch(mask) {
		case IRQ_MASK:
			asm volatile("cpsie i");
			break;
		case FIQ_MASK:
			asm volatile("cpsie f");
			break;
		case ALL_INTERRUPT_MASK:
			asm volatile("cpsie if");
			print_uart0("Enable All Interrupts\n");
			break;
	}
}


/* disable IRQ and/or FIQ */
void disable_interrupt(interrupt_t mask) {
	switch(mask) {
		case IRQ_MASK:
			asm volatile("cpsid i");
			break;
		case FIQ_MASK:
			asm volatile("cpsid f");
			break;
		case ALL_INTERRUPT_MASK:
			asm volatile("cpsid if");
			print_uart0("Disable All Interrupts\n");
			break;
	}
}

/* disable IRQ and/or FIQ, but also return a copy of the CPSR */
int disable_interrupt_save(interrupt_t mask) {
	/* get a copy of the current process status register */
	int cpsr;
	asm volatile("mrs %0, cpsr" : "=r"(cpsr));

	switch(mask) {
		case IRQ_MASK:
			asm volatile("cpsid i");
			break;
		case FIQ_MASK:
			asm volatile("cpsid f");
			break;
		case ALL_INTERRUPT_MASK:
			asm volatile("cpsid if");
			break;
	}
	return cpsr;
}

/* return a full 32-bit copy of the current process status register */
int get_proc_status(void) {
	int cpsr;
	asm volatile("mrs %0, cpsr" : "=r"(cpsr));
	print_uart0("Getting Process Status\n");
	return cpsr;
}

/* restore control status (interrupt, mode bits) of the cpsr */
/* (e.g. when we return from a handler, restore value from 
   disable_interrupt_save				     */
void restore_proc_status(int cpsr) {
	asm volatile("msr cpsr_c, %0" : : "r"(cpsr));
}
