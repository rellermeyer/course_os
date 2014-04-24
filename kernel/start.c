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

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))
void uart_handler(void *null) {
	print_uart0("uart0!\n");
}

void start(void *p_bootargs) {
  char *cmdline_args = read_cmdline_tag(p_bootargs);

  print_uart0("arguments: ");
  print_uart0(cmdline_args);
  print_uart0("\n");
  print_uart0("CourseOS!\n");

  // Separate the command-line arguments into separate Strings
  int num_args = number_of_words(cmdline_args);
  char* arg_list[num_args];
  split_string(cmdline_args, arg_list);
  int arg_count = sizeof(arg_list) / sizeof(arg_list[0]);
    for (i = 0; i < callNumber; i++) {
        print_uart0("success!\n");
    }
}

void start() {
    print_uart0("CourseOS!\n");

    /* Setup primary vector table */
    *(uint32_t volatile *)(0x00) = (LDR_PC_PC | 0x18); // Reset
    *(uint32_t volatile *)(0x04) = (LDR_PC_PC | 0x18); // Undefined instruction
    *(uint32_t volatile *)(0x08) = (LDR_PC_PC | 0x18); // Software interrupt (SWI)
    *(uint32_t volatile *)(0x0C) = (LDR_PC_PC | 0x18); // Abort (prefetch)
    *(uint32_t volatile *)(0x10) = (LDR_PC_PC | 0x18); // Abort (data)
    *(uint32_t volatile *)(0x14) = (LDR_PC_PC | 0x18); // Address exception
    *(uint32_t volatile *)(0x18) = (LDR_PC_PC | 0x18); // IRQ
    *(uint32_t volatile *)(0x1C) = (LDR_PC_PC | 0x18); // FIQ

    /* Setup secondary vector table */
    *(uint32_t volatile *)(0x20) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x24) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x28) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x2C) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x30) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x34) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x38) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x3C) = (uint32_t)test_handler;

  // Parse and analyze each String
  parse_arguments(arg_count, arg_list);

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

  //asm volatile("wfi");
   print_uart0("arguments: ");
   print_uart0(read_cmdline_tag(p_bootargs));
   print_uart0("\n");
   print_uart0("CourseOS!\n");

   //initialize GLOBAL_PID and PCB table
   init_all_processes();
   
   init_vector_table();

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
