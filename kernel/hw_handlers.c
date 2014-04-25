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

/* copy vector table from wherever the hell QEMU loads the kernel to 0x00 */
void init_vector_table(void) {
    extern uint32_t vector_table_start, vector_table_end;
    uint32_t *src = &vector_table_start;
    uint32_t *dst = (uint32_t *) HIVECTABLE;

    while(src < &vector_table_end)
	*dst++ = *src++;
}


/* handlers */
void reset_handler(void) {
    print_uart0("RESET HANDLER\n");
    _Reset();
}

void __attribute__((interrupt("UNDEF"))) undef_instruction_handler(void){
	v_printf("UNDEFINED INSTRUCTION HANDLER\n");
}

void  __attribute__((interrupt("SWI"))) software_interrupt_handler(void){
	// int i, callNumber;

	// // the link register currently holds the address of the instruction immediately
	// // after the SVC call
	// // possible that syscall # passed directly in r7, not sure yet though
	// register int address asm("lr"); 
	        
	// // load the SVC call and mask to get the number
	// callNumber = *((uint32_t *)(address-4)) & 0x00FFFFFF;

	print_vuart0("SOFTWARE INTERRUPT HANDLER\n");

	// // Print out syscall # for debug purposes
	// print_vuart0("Syscall #: ");
	// v_printf("%x", &callNumber);
	// print_vuart0("\n");
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
	print_uart0("IRQ HANDLER\n");
	disable_interrupts();	
	// Discover source of interrupt
	uint32_t mask = 1;
	int source = 0;
	// this search will need to become round robbin, but this is good for now
	while (source < MAX_NUM_INTERRUPTS){
		mask = mask << source;
		if (mask == VIC_IRQ_STATUS){
			break;
		}
		source++;
	}
	handle_irq_interrupt(source);
	enable_interrupts();
}

void __attribute__((interrupt("FIQ"))) fiq_handler(void){
	print_vuart0("FIQ HANDLER\n");
// FIQ handler returns from the interrupt by executing:
// SUBS PC, R14_fiq, #4
}
