/*
 *
 * Harware Handler Interface  
 *
 */
#include "include/hw_handlers.h"
#include "include/mmap.h"
#include "include/interrupt.h"

/* copy vector table from wherever the hell QEMU loads the kernel to 0x00 */
void init_vector_table(void) {
    /* This doesn't seem to work well with virtual memory; reverting
     * to old method.       
    extern uint32_t vector_table_start, vector_table_end;
    uint32_t *src = &vector_table_start;
    uint32_t *dst = (uint32_t *) HIVECTABLE;

    while(src < &vector_table_end)
	*dst++ = *src++;
    */

    /* Primary Vector Table */
    mmio_write( 0x00, BRANCH_INSTRUCTION);
    mmio_write( 0x04, BRANCH_INSTRUCTION);
    mmio_write( 0x08, BRANCH_INSTRUCTION);
    mmio_write( 0x0C, BRANCH_INSTRUCTION);
    mmio_write( 0x10, BRANCH_INSTRUCTION);
    mmio_write( 0x14, BRANCH_INSTRUCTION);
    mmio_write( 0x18, BRANCH_INSTRUCTION);
    mmio_write( 0x1C, BRANCH_INSTRUCTION);

    /* Secondary Vector Table */
    mmio_write( 0x20, &reset_handler); 
    mmio_write( 0x24, &undef_instruction_handler ); 
    mmio_write( 0x28, &software_interrupt_handler ); 
    mmio_write( 0x2C, &prefetch_abort_handler ); 
    mmio_write( 0x30, &data_abort_handler ); 
    mmio_write( 0x34, &reserved_handler ); 
    mmio_write( 0x38, &irq_handler ); 
    mmio_write( 0x3C, &fiq_handler ); 
}


/* handlers */
void reset_handler(void) {
    print_uart0("RESET HANDLER\n");
    _Reset();
}

void __attribute__((interrupt("UNDEF"))) undef_instruction_handler(void){
	os_printf("UNDEFINED INSTRUCTION HANDLER\n");
}

void  __attribute__((interrupt("SWI"))) software_interrupt_handler(void){
	// int i, callNumber;

	// // the link register currently holds the address of the instruction immediately
	// // after the SVC call
	// // possible that syscall # passed directly in r7, not sure yet though
	// register int address asm("lr"); 
	        
	// // load the SVC call and mask to get the number
	// callNumber = *((uint32_t *)(address-4)) & 0x00FFFFFF;

	print_uart0("SOFTWARE INTERRUPT HANDLER\n");

	// // Print out syscall # for debug purposes
	// print_uart0("Syscall #: ");
	// os_printf("%x", &callNumber);
	// print_uart0("\n");
}

void __attribute__((interrupt("ABORT"))) prefetch_abort_handler(void){
	os_printf("PREFETCH ABORT HANDLER\n");
}

void __attribute__((interrupt("ABORT"))) data_abort_handler(void){
	os_printf("DATA ABORT HANDLER\n");
  int pc, lr, sp, fp;
  asm volatile("mov %0, pc" : "=r" (pc));
  asm volatile("mov %0, lr" : "=r" (lr));
  asm volatile("mov %0, sp" : "=r" (sp));
  asm volatile("mov %0, fp" : "=r" (fp));
  os_printf("HANDLER: pc=%x, lr=%x, sp=%x, fp=%x\n", pc, lr, sp, fp); 
}

void reserved_handler(void){
	print_uart0("RESERVED HANDLER\n");
}

// the attribute automatically saves and restores state
void __attribute__((interrupt("IRQ"))) irq_handler(void){

	disable_interrupts();
	print_uart0("IRQ HANDLER\n");
	enable_interrupts();
}

void __attribute__((interrupt("FIQ"))) fiq_handler(void){
	print_uart0("FIQ HANDLER\n");
// FIQ handler returns from the interrupt by executing:
// SUBS PC, R14_fiq, #4
}
