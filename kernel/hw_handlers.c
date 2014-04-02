#include "include/interrupt.h"

/*
 *
 * Harware Handler Interface  
 *
 */


/* handlers */
void reset_handler(void){
	print_uart0("RESET\n");
}

void undef_instruction_handler(void){
}

void software_interrupt_handler(void){
}

void prefetch_abort_handler(void){
}

void data_abort_handler(void){
}

void reserved_handler(void){
}

void irq_handler(void){
}

void fiq_handler(void){
}


/* Return a 32-bit copy of the Current Process Status Register */
inline uint32_t get_proc_status(void) {
	uint32_t cpsr;
	asm("mrs %0, cpsr" : "=r"(cpsr));
	return cpsr;
}
