#include "include/pm.h"
#include "include/stdint.h"
#include "include/stdarg.h"
#include "include/interrupt.h"
#include "include/klibc.h"
#include "include/stack.h"

stack *stack = create_stack();

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
void save_registers()
{
  push(stack, asm("r0"));
  push(stack, asm("r1"));
  push(stack, asm("r2"));
  push(stack, asm("r3"));
  push(stack, asm("r4"));
  push(stack, asm("r5"));
  push(stack, asm("r6"));
  push(stack, asm("r7"));
  push(stack, asm("r8"));
  push(stack, asm("r9"));
  push(stack, asm("r10"));
  push(stack, asm("r11"));
  push(stack, asm("r12"));
		   
 
  push(stack, asm("sp"));
  push(stack, asm("lr"));
  push(stack, asm("pc"));
  push(stack, asm("ip"));
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
