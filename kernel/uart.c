#include <stdint.h>

volatile uint32_t *const UART_ADDRESS = (uint32_t *)0x101f1000;
 
void print_uart0(const char *s) {
   while(*s != '\0') { 
      *UART_ADDRESS = (uint32_t)(*s); 
      s++;
   }
}

void print_bits(uint32_t * c) {
	int a = 31;
	for( ; a >= 0; a--)
		*c & (1 << a) ? print_uart0("1") : print_uart0("0");
	print_uart0("\n");
}
