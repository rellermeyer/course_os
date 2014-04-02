#include <stdint.h>

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

}


