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

   os_printf("%x\n", *pcb_table);

   //Testing and Debug Info
   pcb* test = process_create(0x20000, "TEST");
   //os_printf("test: %x\n", test);

   pcb* test2 = process_create(0x20000, "TEST2");
   //os_printf("tst2: %x\n", test2);
  
   pcb* test3 = process_create(0x20000, "TEST3");
   //os_printf("test3: %x\n", test3);

   pcb* test4 = process_create(0x20000, "TEST4");
   //os_printf("test4: %x\n", test4);

   os_printf("%x\n", *pcb_table);
   print_pcb_table();
   
   // int *x = mem_alloc(1);
   // os_printf("x: %x\n", x);
   // int *y = mem_alloc(1);
   // os_printf("y: %x\n", y);
}

