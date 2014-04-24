#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"


void start(void *p_bootargs) {
   print_uart0("arguments: ");
   print_uart0(read_cmdline_tag(p_bootargs));
   print_uart0("\n");
   print_uart0("CourseOS!\n");
   os_printf("test %d %d %d %d\n", 7, 4, 42, -123);
   os_printf("test %x %x %x %x\n", 7, 4, 42, -123);
   os_printf("test %X %X %X %X\n", 7, 4, 42, -123);

   init_heap(32*0x100000);
   test_heap_manager();
}

void test_heap_manager(){
  uint32_t* block0 = kmalloc(8);
  uint32_t* block1 = kmalloc(10);
  uint32_t* block2 = kmalloc(13);
  mcheck();
  os_printf("\n");
  kfree(block0);
  mcheck();
  os_printf("\n");
  kfree(block2);
  mcheck();
  os_printf("\n");
  kfree(block1);
  os_printf("\n");
  mcheck();

}