#include <stdint.h>

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

static const uint32_t LDR_PC_PC = 0xe59ff000;

void test_handler() {
    int i, callNumber;

    // the link register currently holds the address of the instruction immediately
    // after the SVC call
    register int address asm("lr"); 
    
    // load the SVC call and mask to get the number
    callNumber = *((uint32_t *)(address-4)) & 0x00FFFFFF;

    for (i = 0; i < callNumber; i++) {
        print_uart0("success!\n");
    }
}

void start() {
	print_uart0("arguments ");
   	print_uart0(44 + p_bootargs);
   	print_uart0("\n");
   	print_uart0("CourseOS!\n");

	/* we boot into SVC mode with FIQ and IRQ masked */
	/* TODO: intialize the vector table, stack space, etc. */
	asm volatile(
		""
	);
	
	/* testing */
	uint32_t cpsr;
	cpsr = get_proc_status();
	print_word_bits(&cpsr);
	
	// test enabling IRQ
	cpsr = ChangeIRQ(1);
        print_word_bits(&cpsr);

	// test disabling IRQ
	cpsr = ChangeIRQ(0);	
        print_word_bits(&cpsr);

       // test enabling FIQ
        cpsr = ChangeFIQ(1);
        print_word_bits(&cpsr);

        // test disabling FIQ
        cpsr = ChangeFIQ(0);
        print_word_bits(&cpsr);
    
	/* Setup primary vector table */
    *(uint32_t volatile *)(0x00) = (LDR_PC_PC | 0x18);
    *(uint32_t volatile *)(0x04) = (LDR_PC_PC | 0x18);
    *(uint32_t volatile *)(0x08) = (LDR_PC_PC | 0x18);
    *(uint32_t volatile *)(0x0C) = (LDR_PC_PC | 0x18);
    *(uint32_t volatile *)(0x10) = (LDR_PC_PC | 0x18);
    *(uint32_t volatile *)(0x14) = (LDR_PC_PC | 0x18);
    *(uint32_t volatile *)(0x18) = (LDR_PC_PC | 0x18);
    *(uint32_t volatile *)(0x1C) = (LDR_PC_PC | 0x18);

    /* Setup secondary vector table */
    *(uint32_t volatile *)(0x20) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x24) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x28) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x2C) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x30) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x34) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x38) = (uint32_t)test_handler;
    *(uint32_t volatile *)(0x3C) = (uint32_t)test_handler;

}
