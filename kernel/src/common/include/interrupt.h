#ifndef __HW_HANDLERS_H__
#define __HW_HANDLERS_H__

/*
 *
 *  Hardware Handler Interface for course_os
 *
 *
 *  A bit of background:
 *  - The ARM architecture has 7 modes of operation:
 *  + USR - User mode
 *  + FIQ - processing "fast" interrupts
 *  + IRQ - processing "normal" interrupts
 *  + SVC - Supervisor/Kernel mode
 *  + UND - processing an undefined instruction exception
 *  + SYS - Privileged mode but retains the registers of the user mode, used for handling interrupts
 * in a faster way. These modes can be entered or exited by modifying the CPSR (status register)
 *
 *  All modes have their own stack which can be found in [stacks.s](stacks.s]
 *
 *  Exceptions (e.g. software interrupts, system calls, etc.), Interrupts (IRQ, FIQ)
 *  trigger the core to switch to an appropriate mode and the pc to jump to a
 *  preset address (somewhere in the vector table) for a branch instruction to the
 *  proper handler.
 *
 *  When this happens the state of the machine must be preserved. The HW handler interface
 *  centralizes the 'top half' exception/interrupt handling code and takes care of the dirty
 * low-level work so that the software handling interfaces for interrupts, system calls, and
 * exceptions can be written more clearly elsewhere.
 *
 *  tl;dr - write your handlers as a separate module and call them from one
 *  of the prototypes below.
 *
 */
#include "stdint.h"

#define BRANCH_INSTRUCTION 0xe59ff018  // ldr pc, pc+offset (where offset is 0x20 bytes)

void init_vector_table(void);

// vector table handlers, should be loaded at 0x00 in this order!
extern void _Reset();

void reset_handler(void);

void __attribute__((interrupt("UNDEF"))) undef_instruction_handler();  // 0x04
void swi_handler(void * registers);                                    // Initially handled in asm
void __attribute__((interrupt("ABORT"))) prefetch_abort_handler();     // 0x0c
void __attribute__((interrupt("ABORT"))) data_abort_handler();         // 0x10
void reserved_handler();                                               // 0x14
void __attribute__((interrupt("IRQ"))) irq_handler();                  // 0x18
void __attribute__((interrupt("FIQ"))) fiq_handler();                  // 0x1c

/**
 * Semihosting calls
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0471g/CHDJHHDI.html
 */
enum SemihostingSWI {
    BreakPoint = 0x20020,
    WatchPoint = 0x20021,
    StepComplete = 0x20022,
    RunTimeErrorUnknown = 0x20023,  // Qemu exits with 1
    InternalError = 0x20024,        // Qemu exits with 1
    UserInterruption = 0x20025,     // Qemu exits with 1
    ApplicationExit = 0x20026,      // Qemu exits with 0
    StackOverflow = 0x20027,        // Qemu exits with 1
    DivisionByZero = 0x20028,       // Qemu exits with 1
    OSSpecific = 0x20029,           // Qemu exits with 1
};

void SemihostingCall(enum SemihostingSWI mode);

void SemihostingOSExit(uint8_t code) __attribute__((noreturn));

typedef enum {
    IRQ,  // (this is bit 0x8 on the CPSR)
    FIQ,  // (this is bit 0x4 on the CPSR)
    BOTH
} InterruptType;

void enable_interrupt(InterruptType);
int enable_interrupt_save(InterruptType);
void disable_interrupt(InterruptType);
int disable_interrupt_save(InterruptType);

#endif
