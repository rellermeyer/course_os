#include "include/interrupt.h"

/*
 *
 * Interrupts  
 *
 */

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
  asm(
	"MRS my_cpsr, CPSR \n\t"                      /* get current program status */
  	"ORR my_cpsr, my_cpsr, #0x80 \n\t"            /* set IRQ disable bit flag */
  	"BIC my_cpsr, my_cpsr, NewState, LSL #7 \n\t" /* reset IRQ bit with new value */
  	"MSR CPSR_c, my_cpsr \n\t"                    /* store updated program status */
  );
  return my_cpsr;
}
