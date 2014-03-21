#include <stdint.h>

static const uint32_t LDR_PC_PC = 0xe59ff000;

void test_handler() {
    print_uart0("success!\n");
}

void start() {
   print_uart0("CourseOS!\n");

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


