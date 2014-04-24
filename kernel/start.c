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
#include <stdint.h>
#include "hw_handlers.h"
#include "global_defs.h"
#include "argparse.h"
#include "interrupt.h"
#include "mmap.h"
#include "pmap.h"
#include "vmlayout.h"
#include "timer.h"
#include "process.h"

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))
void uart_handler(void *null) {
	print_uart0("uart0!\n");
}

void start(void *p_bootargs) {
    print_uart0("Init...\n");
    /*
  char *cmdline_args = read_cmdline_tag(p_bootargs);

  print_uart0("arguments: ");
  print_uart0(cmdline_args);
  print_uart0("\n");
  print_uart0("CourseOS!\n");
  */

  //don't allow interrpts messing with memory
  disable_interrupts();
  //setup page table and enable MMU
  mmap();  
  //register handlers
  init_vector_table();
  interrupt_handler_t uart0_handler_struct = { &uart_handler };
  register_interrupt_handler(UART0_IRQ, &uart0_handler_struct);

  UART0_IMSC = 1<<4;
  VIC_INT_ENABLE = 1<<12;
  enable_interrupts();

   //initialize GLOBAL_PID and PCB table
   init_all_processes();

  //Test: UART0 mapped to the correct virtual address   
  print_vuart0("MMU enabled\n");

  //setup new stack pointers and jump to main
  asm volatile (".include \"stacks.s\"");

   initialize_timers();
   int timer_to_use = 2;
   set_load_value(timer_to_use, 1000);
   set_background_load_value(timer_to_use, 1000);
   start_timer(timer_to_use);
   os_printf("timer %d control = 0x%x\n",timer_to_use, timer_pointers[timer_to_use]->control);
   set_32_bit_mode(timer_to_use);
   os_printf("timer %d control = 0x%x\n",timer_to_use, timer_pointers[timer_to_use]->control);
   int i;
   for(i=0;i<10;i++) {
     os_printf("cur val = %d\n",get_current_value(timer_to_use));
   }
   get_time();
   //timer_start();
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
