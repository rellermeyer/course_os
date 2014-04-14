#include "include/mmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"


__attribute__ ((noreturn)) void setup_stacks(void){
	os_printf("Setting up stack\n");
   
   asm volatile(
      "LDR     R0, =0xf0000000 \n\t"
      "MSR     CPSR_c, #0x13 \n\t"
      "MOV     SP, R0 \n\t"
      "B       main \n\t"
	);

}