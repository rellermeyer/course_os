#include "include/interrupt.h"

/*
 *
 * Interrupts  
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


// SWI are used to call privileged OS routines.
// IRQ are assigned to general purpose interrupts like  periodic timers.
// FIQ is reserved for one single interrupt source that requires fast response time.

/* Return a 32-bit copy of the Current Process Status Register */
inline uint32_t get_proc_status(void) {
	uint32_t cpsr;
	asm("mrs %0, cpsr" : "=r"(cpsr));
	return cpsr;
}

/* NewState=1 will enable IRQ, NewState=0 will disable IRQ 
   ARM core must be in a privileged mode, e.g. supervisor  */

int ChangeIRQ(unsigned int NewState)
{
  int my_cpsr;
  NewState = NewState << 7;
  asm volatile
  (
	"MRS %[result], CPSR \n\t"                 /* get current program status */
  	"ORR %[result], %[value], #0x80 \n\t"      /* set IRQ disable bit flag */
  	"BIC %[result], %[value], %[state] \n\t"   /* reset IRQ bit with new value */
	"MSR CPSR_c, %[value] \n\t"                /* store updated program status */
		: [result]"=r"(my_cpsr)
		: [value]"r"(my_cpsr), [state]"r"(NewState)
		: // no clobber list needed
  );
  return my_cpsr;
}

/* NewState=1 will enable FIQ, NewState=0 will disable FIQ
   ARM core must be in a privileged mode, e.g. supervisor  */

int ChangeFIQ(unsigned int NewState)
{
  int my_cpsr;
  NewState = NewState << 6;
  asm volatile
  (
        "MRS %[result], CPSR \n\t"                 /* get current program status */
        "ORR %[result], %[value], #0x40 \n\t"      /* set IRQ disable bit flag */
        "BIC %[result], %[value], %[state] \n\t"   /* reset IRQ bit with new value */
        "MSR CPSR_c, %[value] \n\t"                /* store updated program status */
                : [result]"=r"(my_cpsr)
                : [value]"r"(my_cpsr), [state]"r"(NewState)
                : // no clobber list needed
  );
  return my_cpsr;
}
