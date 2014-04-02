#include <stdint.h>

typedef volatile struct {
/* Register info: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0183g/I18381.html */
 uint32_t UARTDR; 		// Data Register
 uint32_t UARTRSR_UARTECR; 	// Receive Status Register/Error Clear Register
 uint8_t reserved1[0x10];	// Reserved Space
 const uint32_t UARTFR; 	// Flag Register
 uint8_t reserved2[0x4];	// Reserved Space
 uint32_t UARTLPR; 		// Low-Power Counter Register
 uint32_t UARTIBRD; 		// Integer Baud Rate Register
 uint32_t UARTFBRD; 		// Fractional Baud Rate Register
 uint32_t UARTLCR_H; 		// Line Control Register
 uint32_t UARTCR; 		// Control Register
 uint32_t UARTIFLS; 		// Interrupt FIFO Level Select Register
 uint32_t UARTIMSC; 		// Interrupt Mask Set/Clear Register
 const uint32_t UARTRIS; 	// Raw Interrupt Status Register
 const uint32_t UARTMIS; 	// Masked Interrupt Status Register
 uint32_t UARTICR; 		// Interrupt Clear Register
 uint32_t UARTDMACR; 		// DMA Control Register
} pl011_T;
 
enum {
/* UARTFR Reg. Info:  http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0183g/I28167.html */
 RXFE = 0x10,
 TXFF = 0x20,
};
 
pl011_T * const UART0 = (pl011_T *)0x101f1000;
pl011_T * const UART1 = (pl011_T *)0x101f2000;
pl011_T * const UART2 = (pl011_T *)0x101f3000;
 
static inline char upperchar(char c) {
 if((c >= 'a') && (c <= 'z')) {
  return c - 'a' + 'A';
 } else {
  return c;
 }
}
 
static void uart_echo(pl011_T *uart) {
 if ((uart->UARTFR & RXFE) == 0) {
  while(uart->UARTFR & TXFF);
  uart->UARTDR = upperchar(uart->UARTDR);
 }
}
