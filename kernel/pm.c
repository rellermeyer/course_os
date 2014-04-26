#include "include/pm.h"
#include "include/stdint.h"
#include "include/stdarg.h"
#include "include/interrupt.h"
#include "include/klibc.h"

//method to set CPU to idle
int cpu_idle(int *fp)
{
	return 0;
}

//method to return CPU to regular operation
int cpu_resume(int *fp)
{
	return 0;
}

//method to save registers on stack
void save_registers(int *fp, int ptrsize, int sp, int *save_ptr)
{

}

//method to clear L1
void clear_cache()
{	
	/* still need to push & pop the stack */
	/* disable cache */
	asm volatile("mrc p15, 0, r0, c1, c0, 0"); //mv control register to r0
	asm volatile("bic r0, #4"); //!& with 4 - clears bit 2, cache bit
	asm volatile("mcr p15, 0, r0, c1, c0, 0"); //write r0 to control register
	
	/* clear cache */
}

void enable_cache()
{
	/* enable cache */
	asm volatile("mrc p15, 0, r0, c1, c0, 0"); //mv control register to r0
	asm volatile("bic r0, #4"); //!& with 4 - enables bit 2, cache bit
	asm volatile("mcr p15, 0, r0, c1, c0, 0"); //write r0 to control register
}
