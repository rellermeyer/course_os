#include "include/interrupt.h"

/*
 *
 * Interrupts  
 *
 */

#include <interrupt.h>

/* Return a 32-bit copy of the Current Process Status Register */
inline uint32_t get_proc_status(void) {
	uint32_t cpsr;
	//asm("mrs %0, cpsr" : "=r"(cpsr));
	return cpsr;
} 
