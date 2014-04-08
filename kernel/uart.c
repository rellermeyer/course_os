#include <stdint.h>

volatile uint32_t *const UART_ADDRESS = (uint32_t *)0x101f1000;

void print_uart0(const char *s) {
  while (*s != '\0') {
    *UART_ADDRESS = (uint32_t)(*s);
    s++;
  }
}
<<<<<<< HEAD

void print_char_uart0(char c) {
  *UART_ADDRESS = (uint32_t) c;
}
=======
>>>>>>> master
