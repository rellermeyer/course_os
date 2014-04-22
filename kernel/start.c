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


void start(void *p_bootargs) {
   char *cmdline_args = read_cmdline_tag(p_bootargs);

   print_uart0("arguments: ");
   print_uart0(cmdline_args);
   print_uart0("\n");
   print_uart0("CourseOS!\n");


   // Separate the command-line arguments into separate Strings
   //int num_args = number_of_words(cmdline_args);
   // char* arg_list[num_args];
   // split_string(cmdline_args, arg_list);
   // int arg_count = sizeof(arg_list) / sizeof(arg_list[0]);

   // Parse and analyze each String
   //parse_arguments(arg_count, arg_list);


   // init_pcb_table();
   
   // init_vector_table();

   // mmap();

   // //Test: UART0 mapped to the correct virtual address   
   // print_vuart0("Virtual Memory!!!\n");

   // //setup new stack pointers and jump to main
   // asm volatile (".include \"stacks.s\"");
  
   // /* NOTHING EXECUTED BEYOND THIS POINT
   //  *
   //  *
   //  * Anything that needs to be setup right after
   //  * booting the kernel should go before mmap()
   //  *
   //  * Any setup, heap allocation or stack allocation
   //  * goes in main
   //  *
   //  */



}
