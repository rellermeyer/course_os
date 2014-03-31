void start(void *p_bootargs) {
   print_uart0("arguments ");
   print_uart0(44 + p_bootargs);
   print_uart0("\n");
   print_uart0("CourseOS!\n");
}

