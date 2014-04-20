/*
 *  A bit of background:
 *  - The ARM architecture has 7 modes of operation:
 *      + USR - user mode
 *      + FIQ - processing "fast" interrupts
 *      + IRQ - processing "normal" interrupts
 *      + SVC - proctected mode for OS
 *      + UND - processing an undefined instruction exception
 *      + SYS - also protecteed mode for OS --if anyone wants to clarify, feel free--
 *
 *  - These modes can be entered or exited by modifying the CPSR (status register), first 5 bits
 *	+ 0b10000 = user mode
 *	+ 0b10001 = FIQ (fast interrupt) mode
 *	+ 0b10010 = IRQ (normal interrupt) mode
 *	+ 0b10011 = SVC (supervisor, or, OS) mode
 *	(others...)		
 */

#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"
#include "data_structures/tests/linked_list_test.c"
#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"

void start(void *p_bootargs) {
   print_uart0("arguments: ");
   print_uart0(read_cmdline_tag(p_bootargs));
   print_uart0("\n");
   print_uart0("CourseOS!\n");
   init_vector_table();
   linked_list_test();

   mmap();

   //Test: UART0 mapped to the correct virtual address   
   print_vuart0("Virtual Memory!!!\n");

   //setup new stack pointers and jump to main
   asm volatile (".include \"stacks.s\"");
  
   /* NOTHING EXECUTED BEYOND THIS POINT
    *
    *
    * Anything that needs to be setup right after
    * booting the kernel should go before mmap()
    *
    * Any setup, heap allocation or stack allocation
    * goes in main
    *
    */
}
