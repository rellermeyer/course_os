#include <interrupt.h>

void start() {
   print_uart0("CourseOS!\n");

	print_uart0("cpsr:\n");
	int a = get_proc_status();
	print_bits(&a);
}

