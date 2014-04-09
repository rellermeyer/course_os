/*
 *
 * Interrupts  
 *
 */
#include "include/interrupt.h"

// In a system with an interrupt controller with these features, software is still required to:
// determine from the interrupt controller which interrupt source is requesting service
// determine where the service routine for that interrupt source is loaded
// mask or clear that interrupt source, before re-enabling processor interrupts to permit another interrupt to be taken.
// 
// An interrupt occurs.
inline void handle_interrupt(int interrupt_vector){
// 	switch(interrupt_vector) {
// 
//         	case :
//                 default: // error
//                         break;
//         }
}


/* enable IRQ and/or FIQ */
inline void enable_interrupt(interrupt_t mask) {
	switch(mask) {
		case IRQ_MASK:
			asm volatile("cpsie i");
			break;
		case FIQ_MASK:
			asm volatile("cpsie f");
			break;
		case ALL_INTERRUPT_MASK:
			asm volatile("cpsie if");
			break;
	}
}


/* disable IRQ and/or FIQ */
inline void disable_interrupt(interrupt_t mask) {
	switch(mask) {
		case IRQ_MASK:
			asm volatile("cpsid i");
			break;
		case FIQ_MASK:
			asm volatile("cpsid f");
			break;
		case ALL_INTERRUPT_MASK:
			asm volatile("cpsid if");
			break;
	}
}

/* disable IRQ and/or FIQ, but also return a copy of the CPSR */
inline int disable_interrupt_save(interrupt_t mask) {
	/* get a copy of the current process status register */
	int cpsr;
	asm volatile("mrs %0, cpsr" : "=r"(cpsr));

	switch(mask) {
		case IRQ_MASK:
			asm volatile("cpsid i");
			break;
		case FIQ_MASK:
			asm volatile("cpsid f");
			break;
		case ALL_INTERRUPT_MASK:
			asm volatile("cpsid if");
			break;
	}
	return cpsr;
}

/* return a full 32-bit copy of the current process status register */
inline int get_proc_status(void) {
	int cpsr;
	asm volatile("mrs %0, cpsr" : "=r"(cpsr));
	return cpsr;
}

/* restore control status (interrupt, mode bits) of the cpsr */
/* (e.g. when we return from a handler, restore value from 
   disable_interrupt_save				     */
inline void restore_proc_status(int cpsr) {
	asm volatile("msr cpsr_c, %0" : : "r"(cpsr));
}


/* I commented this code out because it was throwing a weird
   "bad instruction" error during assembly which I don't know 
   how to debug */

// returns -1 if error, else the final state of the CPSR
//int enableInterrupt(interrupt_t interRequest){
//  int my_cpsr; // stores the CPSR
//  unsigned int newState; // mask for enabling IRQ and FIQ bits
  
  // CRITICAL SECIION -- NEED TO IMPLEMTN MUTEX/LOCKS

  // get the current value in CPSR and store it into my_cpsr
//  asm volatile
//  (
//	"MRS %[result], CPSR \n\t" /* get current program status */
//                : [result]"=r"(my_cpsr)
//                : // input is CPSR
//                : // no clobber list needed
//  );

  /* checking for correctness (can't enable something that's already enabled, etc.) */
//  if  ((interRequest == IRQ) && (((my_cpsr >> 7) & 1) == 0) || (interRequest == FIQ) && (((my_cpsr >> 6) & 1) == 0) || (interRequest == ALL) && ((((my_cpsr >> 6) & 11) != 11)))
//  {
//	return -1;
//  }
  
//  asm volatile
//  (
//        "XOR %[result], %[value], %[mask] \n\t"      /* set IRQ disable bit flag */
// 		case FIQ_MASK:
// 			asm volatile("cpsie f");
// 			break;
// 		case ALL_INTERRUPT_MASK:
// 			asm volatile("cpsie if");
// 			break;
// 	}
// }
