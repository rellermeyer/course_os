/*
 *
 * Harware Handler Interface  
 *
 */
#include "include/hw_handlers.h"
#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h" 
#include "include/interrupt.h"
#include "include/vmlayout.h"

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
	print_vuart0("RESET HANDLER\n");
}

void __attribute__((interrupt("UNDEF"))) undef_instruction_handler(void){
	print_vuart0("UNDEFINED INSTRUCTION HANDLER\n");
}

void  __attribute__((interrupt("SWI"))) software_interrupt_handler(void){
	int i, callNumber;

	// the link register currently holds the address of the instruction immediately
	// after the SVC call
	// possible that syscall # passed directly in r7, not sure yet though
	register int address asm("lr"); 
	        
	// load the SVC call and mask to get the number
	callNumber = *((uint32_t *)(address-4)) & 0x00FFFFFF;

	print_vuart0("SOFTWARE INTERRUPT HANDLER\n");

	// Print out syscall # for debug purposes
	print_vuart0("Syscall #: ");
	print_word_hex(&callNumber);
	print_vuart0("\n");
}

void __attribute__((interrupt("ABORT"))) prefetch_abort_handler(void){
	v_printf("PREFETCH ABORT HANDLER\n");
}

void __attribute__((interrupt("ABORT"))) data_abort_handler(void){
	v_printf("DATA ABORT HANDLER\n");
  int pc, lr, sp, fp;
  asm volatile("mov %0, pc" : "=r" (pc));
  asm volatile("mov %0, lr" : "=r" (lr));
  asm volatile("mov %0, sp" : "=r" (sp));
  asm volatile("mov %0, fp" : "=r" (fp));
  v_printf("HANDLER: pc=%x, lr=%x, sp=%x, fp=%x\n", pc, lr, sp, fp); 
}

void reserved_handler(void){
	print_vuart0("RESERVED HANDLER\n");
}

// the attribute automatically saves and restores state
void __attribute__((interrupt("IRQ"))) irq_handler(void){		
	if (*PIC_ADDRESS == 1)       // which interrupt was it?
	{
	    /* Commenting this call out until it actually gets defined;
	     * otherwise compiling fails with new Makefile 

		handle_interrupt(*PIC_ADDRESS);  // process the interrupt

	    */
	}
	*(PIC_ADDRESS+1) = *PIC_ADDRESS;    // clear the interrupt
}

void __attribute__((interrupt("FIQ"))) fiq_handler(void){
	print_vuart0("FIQ HANDLER\n");
// FIQ handler returns from the interrupt by executing:
// SUBS PC, R14_fiq, #4
}
