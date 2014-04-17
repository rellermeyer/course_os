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


void start(void *p_bootargs) {
   print_uart0("arguments: ");
   print_uart0(read_cmdline_tag(p_bootargs));
   print_uart0("\n");
   print_uart0("CourseOS!\n");
   os_printf("test %d %d %d %d\n", 7, 4, 42, -123);
   os_printf("test %x %x %x %x\n", 7, 4, 42, -123);
   os_printf("test %X %X %X %X\n", 7, 4, 42, -123);

    /* we boot into SVC mode with FIQ and IRQ masked */
    /* TODO: intialize the vector table, stack space, etc. */
    init_vector_table();
}
