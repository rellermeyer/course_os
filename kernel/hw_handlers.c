/*
 *
 * Harware Handler Interface  
 *
 */
#include "hw_handlers.h"
#include "mmap.h"
#include "vmlayout.h"

void init_vector_table(void) {

	/* Primary Vector Table */
	mmio_write(HIVECTABLE | 0x00, BRANCH_INSTRUCTION);
	mmio_write(HIVECTABLE | 0x04, BRANCH_INSTRUCTION);
	mmio_write(HIVECTABLE | 0x08, BRANCH_INSTRUCTION);
	mmio_write(HIVECTABLE | 0x0C, BRANCH_INSTRUCTION);
	mmio_write(HIVECTABLE | 0x10, BRANCH_INSTRUCTION);
	mmio_write(HIVECTABLE | 0x14, BRANCH_INSTRUCTION);
	mmio_write(HIVECTABLE | 0x18, BRANCH_INSTRUCTION);
	mmio_write(HIVECTABLE | 0x1C, BRANCH_INSTRUCTION);

	/* Secondary Vector Table */
	mmio_write(HIVECTABLE | 0x20, reset_handler);
	mmio_write(HIVECTABLE | 0x24, undef_instruction_handler);
	mmio_write(HIVECTABLE | 0x28, software_interrupt_handler);
	mmio_write(HIVECTABLE | 0x2C, prefetch_abort_handler);
	mmio_write(HIVECTABLE | 0x30, data_abort_handler);
	mmio_write(HIVECTABLE | 0x34, reserved_handler);
	mmio_write(HIVECTABLE | 0x38, irq_handler);
	mmio_write(HIVECTABLE | 0x3C, fiq_handler);
}


/* handlers */
void reset_handler(void){
	print_uart0("RESET HANDLER\n");
}

void undef_instruction_handler(void){
	print_uart0("UNDEFINED INSTRUCTION HANDLER\n");
}

void software_interrupt_handler(void){
	int i, callNumber;

	// the link register currently holds the address of the instruction immediately
	// after the SVC call
	// possible that syscall # passed directly in r7, not sure yet though
	register int address asm("lr"); 
	        
	// load the SVC call and mask to get the number
	callNumber = *((uint32_t *)(address-4)) & 0x00FFFFFF;

	print_uart0("SOFTWARE INTERRUPT HANDLER\n");

	// Print out syscall # for debug purposes
	print_uart0("Syscall #: ");
	print_word_hex(&callNumber);
	print_uart0("\n");
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

void irq_handler(void){
	print_uart0("IRQ HANDLER\n");
}

void fiq_handler(void){
	print_uart0("FIQ HANDLER\n");
}
