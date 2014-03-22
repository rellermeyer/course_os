/*
 *
 * Interrupts  
 *
 */

/* Return a 32-bit copy of the Current Process Status Register */
inline unsigned uint32_t get_proc_status(void) {
	uint32_t cpsr;
	asm("mrs %0, cpsr" : "=r"(cpsr));
	return cpsr;
} 
