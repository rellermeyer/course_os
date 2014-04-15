/*
 *
 * Harware Handler Interface  
 *
 */
#include "include/hw_handlers.h"
#include "include/mmap.h"
#include "include/interrupt.h"


void init_vector_table(void) {

	/* Primary Vector Table */
	mmio_write(0x00, BRANCH_INSTRUCTION);
	mmio_write(0x04, BRANCH_INSTRUCTION);
	mmio_write(0x08, BRANCH_INSTRUCTION);
	mmio_write(0x0C, BRANCH_INSTRUCTION);
	mmio_write(0x10, BRANCH_INSTRUCTION);
	mmio_write(0x14, BRANCH_INSTRUCTION);
	mmio_write(0x18, BRANCH_INSTRUCTION);
	mmio_write(0x1C, BRANCH_INSTRUCTION);

	/* Secondary Vector Table */
	mmio_write(0x20, reset_handler);
	mmio_write(0x24, undef_instruction_handler);
	mmio_write(0x28, software_interrupt_handler);
	mmio_write(0x2C, prefetch_abort_handler);
	mmio_write(0x30, data_abort_handler);
	mmio_write(0x34, reserved_handler);
	mmio_write(0x38, irq_handler);
	mmio_write(0x3C, fiq_handler);
}


/* handlers */
void reset_handler(void){
	print_uart0("RESET HANDLER\n");
}

void undef_instruction_handler(void){
	print_uart0("UNDEFINED INSTRUCTION HANDLER\n");
}

void  __attribute__((interrupt("SWI"))) software_interrupt_handler(void){
	print_uart0("SOFTWARE INTERRUPT HANDLER\n");
}

void prefetch_abort_handler(void){
	print_uart0("PREFETCH ABORT HANDLER\n");
}

void data_abort_handler(void){
	print_uart0("DATA ABORT HANDLER\n");
}

void reserved_handler(void){
	print_uart0("RESERVED HANDLER\n");
}

void __attribute__((interrupt("IRQ"))) irq_handler(void){		
	volatile unsigned int *base = (unsigned int *) 0x80000000;
	if (*base == 1)       // which interrupt was it?
	{
		handle_interrupt(1);  // process the interrupt
	}
	*(base+1) = *base;    // clear the interrupt
}

void __attribute__((interrupt("FIQ"))) fiq_handler(void){
	print_uart0("FIQ HANDLER\n");
// FIQ handler returns from the interrupt by executing:
// SUBS PC, R14_fiq, #4
}
