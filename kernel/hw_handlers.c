/*
 *
 * Harware Handler Interface  
 *
 */
#include "include/hw_handlers.h"
#include "include/mmap.h"
#include "include/interrupt.h"
#include "include/vmlayout.h"

/* copy vector table from wherever the hell QEMU loads the kernel to 0x00 */
void init_vector_table(void) {
	extern uint32_t vector_table_start, vector_table_end;
	uint32_t *src = &vector_table_start;
	uint32_t *dst = (uint32_t *) 0x00;

	while(src < &vector_table_end)
		*dst++ = *src++;
}


/* handlers */

// the reset handler is in startup.s

void __attribute__((interrupt("UNDEF"))) undef_instruction_handler(void){
	print_uart0("UNDEFINED INSTRUCTION HANDLER\n");
}

void  __attribute__((interrupt("SWI"))) software_interrupt_handler(void){
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

void __attribute__((interrupt("ABORT"))) prefetch_abort_handler(void){
	print_uart0("PREFETCH ABORT HANDLER\n");
}

void __attribute__((interrupt("ABORT"))) data_abort_handler(void){
	print_uart0("DATA ABORT HANDLER\n");
}

void reserved_handler(void){
	print_uart0("RESERVED HANDLER\n");
}

// the attribute automatically saves and restores state
void __attribute__((interrupt("IRQ"))) irq_handler(void){

	print_uart0("IRQ HANDLER\n");
	disable_interrupts();	

	// Discover source of interrupt
	int i = 0;
	// do a straight run through the VIC_INT_STATUS to determine
	// which interrupt lines need to be tended to
	for(i = 0; i < MAX_NUM_INTERRUPTS; i++){
		// is the line active?
		if((1 << i) & mmio_read(VIC_IRQ_STATUS)) {
			// activate that specific handler
			handle_irq_interrupt(i);	
		}
	}
	// we've gone through the VIC and handled all active interrupts
	enable_interrupts();
}

void __attribute__((interrupt("FIQ"))) fiq_handler(void){
	print_uart0("FIQ HANDLER\n");
// FIQ handler returns from the interrupt by executing:
// SUBS PC, R14_fiq, #4
}
