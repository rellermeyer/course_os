#include <stdint.h>

#define STD_IN_BUFFER_SIZE 4096

/* Contributors: David Denton, Michael Brennan */

enum {
/* UART inter: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0224i/I1042232.html */
PICINTENABLE = 0x10140010,
UART0INT_BIT = 0x0800,
UART1INT_BIT = 0x1000,
UART2INT_BIT = 0x2000
};


typedef volatile struct {
uint32_t uart0_inter_val = *PICINTENABLE | UART0INT_BIT;
uint32_t uart1_inter_val = *PICINTENABLE | UART1INT_BIT;
uint32_t uart2_inter_val = *PICINTENABLE | UART2INT_BIT;
} device_driver;

typedef volatile struct {
/* Register info: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0183g/I18381.html */
device_driver *dd;
 uint32_t UARTDR;     // Data Register
 uint32_t UARTRSR_UARTECR;  // Receive Status Register/Error Clear Register
 uint8_t reserved1[0x10]; // Reserved Space
 const uint32_t UARTFR;   // Flag Register
 uint8_t reserved2[0x4];  // Reserved Space
 uint32_t UARTLPR;    // Low-Power Counter Register
 uint32_t UARTIBRD;     // Integer Baud Rate Register
 uint32_t UARTFBRD;     // Fractional Baud Rate Register
 uint32_t UARTLCR_H;    // Line Control Register
 uint32_t UARTCR;     // Control Register
 uint32_t UARTIFLS;     // Interrupt FIFO Level Select Register
 uint32_t UARTIMSC;     // Interrupt Mask Set/Clear Register
 const uint32_t UARTRIS;  // Raw Interrupt Status Register
 const uint32_t UARTMIS;  // Masked Interrupt Status Register
 uint32_t UARTICR;    // Interrupt Clear Register
 uint32_t UARTDMACR;    // DMA Control Register
} uart;

// UARTFR reg. enums
enum {
RXFE = 0x10,
RXFF = 0x40, 
TXFF = 0x20,
TXFE = 0x80,
BUSY = 0x08,
};

// UARTCF reg. enums
enum {
TXE = 0x100,
RXE = 0x200
}

uart * const UART0 = (uart *)0x101f1000;
uart * const UART1 = (uart *)0x101f2000;
uart * const UART2 = (uart *)0x101f3000;

void print_uart0(const char *s) {
  while (uart->dd->uart0_inter_val == 0) {}
  if(uart.UARTCR & RXE > 0) 
  {
    while(*s != '\0') 
    {
      *UART0 = (uint32_t)(*s);
      s++;
    }
  }
}

void print_char_uart0(char c)
{
  *UART_ADDRESS = (uint32_t)c;
}
