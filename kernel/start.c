#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"
#include "include/klibc.h"
#include "include/process.h"

void start(void *p_bootargs) {
   print_uart0("arguments: ");
   print_uart0(read_cmdline_tag(p_bootargs));
   print_uart0("\n");
   print_uart0("CourseOS!\n");
   os_printf("test %d %d %d %d\n", 7, 4, 42, -123);
   os_printf("test %x %x %x %x\n", 7, 4, 42, -123);
   os_printf("test %X %X %X %X\n", 7, 4, 42, -123);


   //initialize GLOBAL_PID
   init_all_processes();

   //Testing and Debug Info
   pcb* test = process_create(0x20000, "TEST");
   print_uart0(test->name);
   print_uart0("\n");
   os_printf("%d\n", test->PID);
   os_printf("%x\n", test);

   pcb* test2 = process_create(0x20000, "TEST2");
   print_uart0(test2->name);
   print_uart0("\n");
   os_printf("%d\n", test2->PID);

   os_printf("%d\n", test->PID);

   os_printf("%x\n", test2);

   int *x = mem_alloc(1);
   os_printf("%x\n", x);
   int *y = mem_alloc(1);
   os_printf("%x\n", y);
}

