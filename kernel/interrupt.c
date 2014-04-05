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

// returns -1 if error, else the final state of the CPSR
int enableInterrupt(enum inter_os interRequest){
  int my_cpsr; // stores the CPSR
  unsigned int newState; // mask for enabling IRQ and FIQ bits
  
  // CRITICAL SECIION -- NEED TO IMPLEMTN MUTEX/LOCKS

  // get the current value in CPSR and store it into my_cpsr
  asm volatile
  (
	"MRS %[result], CPSR \n\t" /* get current program status */
                : [result]"=r"(my_cpsr)
                : // input is CPSR
                : // no clobber list needed
  );

  /* checking for correctness (can't enable something that's already enabled, etc.) */
  if  ((interRequest == IRQ) && (((my_cpsr >> 7) & 1) == 0) || (interRequest == FIQ) && (((my_cpsr >> 6) & 1) == 0) || (interRequest == ALL) && ((((my_cpsr >> 6) & 11) != 11)))
  {
	return -1;
  }
  
  asm volatile
  (
        "XOR %[result], %[value], %[mask] \n\t"      /* set IRQ disable bit flag */
        "MSR CPSR_c, %[value] \n\t"                /* store updated program status */
                : [result]"=r"(my_cpsr)
                : [value]"r"(my_cpsr), [mask]"r"(interRequest)
                : // no clobber list needed
  );
  
  // END OF CRITICAL SECIION -- NEED TO IMPLEMTN MUTEX/LOCKS  

  return my_cpsr;
}


/* NewState=1 will enable IRQ, NewState=0 will disable IRQ 
   ARM core must be in a privileged mode, e.g. supervisor  */

//int ChangeIRQ(unsigned int NewState)
//{
//  int my_cpsr;
//  NewState = NewState << 7;
//  asm volatile
// (
//	"MRS %[result], CPSR \n\t"                 /* get current program status */
//  	"ORR %[result], %[value], #0x80 \n\t"      /* set IRQ disable bit flag */
// 	"BIC %[result], %[value], %[state] \n\t"   /* reset IRQ bit with new value */
//	"MSR CPSR_c, %[value] \n\t"                /* store updated program status */
//		: [result]"=r"(my_cpsr)
//		: [value]"r"(my_cpsr), [state]"r"(NewState)
//		: // no clobber list needed
//  );
//  return my_cpsr;
//}

/* NewState=1 will enable FIQ, NewState=0 will disable FIQ
   ARM core must be in a privileged mode, e.g. supervisor  */

// int ChangeFIQ(unsigned int NewState)
//{
//  int my_cpsr;
//  NewState = NewState << 6;
//  asm volatile
//  (
//       "MRS %[result], CPSR \n\t"                 /* get current program status */
//        "ORR %[result], %[value], #0x40 \n\t"      /* set IRQ disable bit flag */
//        "BIC %[result], %[value], %[state] \n\t"   /* reset IRQ bit with new value */
//        "MSR CPSR_c, %[value] \n\t"                /* store updated program status */
//                : [result]"=r"(my_cpsr)
//                : [value]"r"(my_cpsr), [state]"r"(NewState)
//                : // no clobber list needed
//  );
//  return my_cpsr;
//}
