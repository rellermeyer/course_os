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
   // os_printf("test %d %d %d %d\n", 7, 4, 42, -123);
   // os_printf("test %x %x %x %x\n", 7, 4, 42, -123);
   // os_printf("test %X %X %X %X\n", 7, 4, 42, -123);


   //initialize GLOBAL_PID and PCB table
   init_all_processes();
	
	load_process_state();


 

}

