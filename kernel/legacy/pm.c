#include "pm.h"
#include "stdint.h"
#include "stdarg.h"
#include "interrupt.h"
#include "klibc.h"
#include "stack.h"

stack *stack = create_stack();

//method to set CPU to idle
int cpu_idle(int *fp)
{
	save_registers();
	clear_cache();
	return 0;
}

//method to return CPU to regular operation
int cpu_resume(int *fp)
{
	enable_cache();
	return 0;
}

//method to save registers on stack
void save_registers()
{
push(stack, asm volatile ("r0"));
push(stack, asm volatile ("r1"));
push(stack, asm volatile ("r2"));
push(stack, asm volatile ("r3"));
push(stack, asm volatile ("r4"));
push(stack, asm volatile ("r5"));
push(stack, asm volatile ("r6"));
push(stack, asm volatile ("r7"));
push(stack, asm volatile ("r8"));
push(stack, asm volatile ("r9"));
push(stack, asm volatile ("r10"));
push(stack, asm volatile ("r11"));
push(stack, asm volatile ("r12"));

push(stack, asm volatile ("ip"));
push(stack, asm volatile ("lr"));
push(stack, asm volatile ("pc"));
push(stack, asm volatile ("sp"));

}

void restore_registers()
{

}

//method to clear L1
void clear_cache()
{
/* still need to push & pop the stack */
/* disable cache */
asm volatile("mov r0, #0");
//clear r0
asm volatile("mrc p15, 0, r0, c1, c0, 0");
//mv control register to r0
asm volatile("bic r0, #4");
//!& with 4 - clears bit 2, cache bit
asm volatile("mcr p15, 0, r0, c1, c0, 0");
//write r0 to control register

/* clean and flush cache */
asm volatile("mov r2, #0");
//clear r2
asm volatile("mcr p15, 0, r2, c7, c14, 0");
//clean and flush data cache
asm volatile("mov r2, #0");
//clear r2
asm volatile("mcr p15, 0, r2, c7, c5, 0");
//clean and flush instruction cache
}

void enable_cache()
{
/* enable cache */
asm volatile("mov r0, #0");
//clear r0
asm volatile("mrc p15, 0, r0, c1, c0, 0");
//mv control register to r0
asm volatile("bic r0, #4");
//!& with 4 - enables bit 2, cache bit
asm volatile("mcr p15, 0, r0, c1, c0, 0");
//write r0 to control register
}
