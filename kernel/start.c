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
#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"
#include "include/interrupt.h"

void *bootargs;

void start(void *p_bootargs) {

  //setup page table and enable MMU
  mmap();

  bootargs = p_bootargs;
  // print_vuart0("arguments: ");
  // print_vuart0(read_cmdline_tag(p_bootargs));
  // print_vuart0("\n");
  print_vuart0("CourseOS!\n");

  //don't allow interrpts messing with memory
  disable_interrupts();
  //register handlers
  init_vector_table();
  enable_interrupts();

  //asm volatile("wfi");

  //Test: UART0 mapped to the correct virtual address   
  print_vuart0("MMU enabled\n");

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
