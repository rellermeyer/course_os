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
#include "drivers/include/timer.h"

#define UART0_DR   (*((volatile uint32_t *)(UART0_ADDRESS + 0x000)))
#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))

void uart_handler(void) {
	print_uart0("uart0!\n");
	/* echo the received character + 1 */
	UART0_DR = UART0_DR + 1;
}


void start(void *p_bootargs) {
	init_vector_table();
	print_uart0("initialized vector table\n");
	asm volatile("SWI 7");

  initialize_timers();

  int timer_to_use = 0;
  os_printf("control default: %x\n",get_timer_control_value(timer_to_use));
  set_load_value(timer_to_use, 25);
  set_background_load_value(timer_to_use, 25);
  set_free_running_mode(timer_to_use);
  enable_timer_interrupt(timer_to_use);
  os_printf("control:  %x\n",get_timer_control_value(timer_to_use));
  start_timer(timer_to_use);
  int i = 0;
  for(i = 0; i < 100; i++){
    os_printf("Timer %d value: %d\n", timer_to_use, get_current_timer_value(timer_to_use));
  }
	
	/* Simulating/Testing Interrupt Routines with UART */
	
	/* THIS PART IS  DONE IN THE DRIVER */
	
	// INITIALIZATION STEPS
	// Step 1: Create handler
	interrupt_handler_t uartHandler;
	uartHandler.handler = uart_handler;
	
	// Step 2: register the handler with interrupts 
	// (12 = interrupt mapping for UART handler -- see interrupt.h)
	register_interrupt_handler(12, &uartHandler);
	
	print_uart0("registed handler\n");
	// INTERRUPT PROCESSING STEPS
	// Step 1: arm interrupts on device
	/* enable RXIM interrupt */
 	UART0_IMSC = 1<<4;
	print_uart0("armed device\n");
	
	/* This stuff should some how happen now */
	// Update the VIC with the appropriate interrupt mapping
	// irq_handler gets called
	

	// now we go off into interrupt land ...
	for(;;); // ... waiting for characher
	// ... ok we're back from the interrupt
	
	// Step 4: disarm the interrupt on the device
	
}
