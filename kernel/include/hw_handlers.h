/*
 *
 *  Hardware Handler Interface for course_os
 *
 * 
 *  A bit of background:
 *  - The ARM architecture has 7 modes of operation:
 * 	+ USR - user mode
 *	+ FIQ - processing "fast" interrupts
 *	+ IRQ - processing "normal" interrupts
 *	+ SVC - proctected mode for OS
 *	+ UND - processing an undefined instruction exception
 *	+ SYS - also protecteed mode for OS --if anyone wants to clarify, feel free--
 *	These modes can be entered or exited by modifying the CPSR (status register)
 *
 *  exceptions (e.g. software interrupts, system calls, etc.), Interrupts (IRQ, FIQ)
 *  trigger the core to switch to an appropriate mode and the pc to jump to a 
 *  preset address (somewhere in the vector table) for a branch instruction to the 
 *  proper handler. 
 *
 *  When this happens the state of the machine must be preserved. The HW handler interface
 *  centralizes the exception/interrupt handling code and takes care of the dirty low-level
 *  work so that the software handling interfaces for interrupts, system calls, and exceptions
 *  can be written more clearly elsewhere.
 *
 *  tl;dr - write your handlers as a separate module and call them from one
 *  of the prototypes below.
 *
 */
#include <stdint.h>


// vector table handlers, should be loaded at 0x00 in this order!
void reset_handler(void);		// 0x00
void undef_instruction_handler(void);	// 0x04
void software_interrupt_handler(void);	// 0x08
void prefetch_abort_handler(void);	// 0x0c
void data_abort_handler(void);		// 0x10
void reserved_handler(void);		// 0x14
void irq_handler(void);			// 0x18
void fiq_handler(void);			// 0x1c

inline uint32_t get_proc_status(void); 
