/*
 *
 * Harware Handler Interface  
 *
 */
#include "include/hw_handlers.h"
#include "include/mmap.h"

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

void software_interrupt_handler(void){
	int i, callNumber;

	// the link register currently holds the address of the instruction immediately
	// after the SVC call
	// possible that syscall # passed directly in r7, not sure yet though
	register int address asm("lr"); 
	        
	// load the SVC call and mask to get the number
	callNumber = *((uint32_t *)(address-4)) & 0x00FFFFFF;

	// once we merge in printf functionality, can print out the syscall #
	// to verify correct parsing.

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

void irq_handler(void){
	print_uart0("IRQ HANDLER\n");
}

void fiq_handler(void){
	print_uart0("FIQ HANDLER\n");
}
