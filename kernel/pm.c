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

//method to clear L1 and L2
void clear_caches(int *fp, int ptrsize, int sp, int *save_ptr)
{

}