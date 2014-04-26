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
#include "memory.h"

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))
void uart_handler(void *null) {
	print_uart0("uart0!\n");
}

void start() {
  print_uart0("\nCourseOS!\n");

  //don't allow interrpts messing with memory
  disable_interrupts(); 
  //register handlers
  init_vector_table();
  interrupt_handler_t uart0_handler_struct = { &uart_handler };
  register_interrupt_handler(UART0_IRQ, &uart0_handler_struct);

  UART0_IMSC = 1<<4;
  VIC_INT_ENABLE = 1<<12;
  enable_interrupts();

  //Test: UART0 mapped to the correct virtual address   
  print_uart0("MMU enabled\n");

  main();
}
