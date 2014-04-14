#include <stdint.h>

#define UART_ADDRESS	(uint32_t *const) 0x101f1000	/* UART 0 base address */
volatile uint32_t *const UART = UART_ADDRESS;

#define UARTV_ADDRESS	(uint32_t *const) 0x801f1000	/* UART 0 virtual base address */
volatile uint32_t *const UARTV = UARTV_ADDRESS;
 
void print_uart0(const char *s) {
   while(*s != '\0') { 
      *UART = (uint32_t)(*s); 
      s++;
   }
}

/* Call this function after MMU is enabled to use UART0 */
void print_vuart0(const char *s) {
   while(*s != '\0') { 
      *UARTV = (uint32_t)(*s); 
      s++;
   }
}


void print_char_uart0(char c) {
  *UARTV_ADDRESS = (uint32_t) c;
}