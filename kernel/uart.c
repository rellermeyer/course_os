#include <stdint.h>

typedef volatile struct {
// Some sort of locking mechanism
} device_driver;

typedef volatile struct {
/* Register info: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0183g/I18381.html */
device_driver *dd;
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
} uart;

enum {
// UARTFR reg. enums
RXFE = 0x10,
RXFF = 0x40, 
TXFF = 0x20,
TXFE = 0x80,
BUSY = 0x08,
TXE = 0x100,
RXE = 0x200,
};

uart * const UART0 = (uart *)0x101f1000;
uart * const UART1 = (uart *)0x101f2000;
uart * const UART2 = (uart *)0x101f3000;

void stdout_uart0(const char *s) {
	if(uart.UARTCR & RXE > 0) {
		while(*s != '\0') {
			*UART0 = (uint32_t)(*s);
			s++;
		}
	}
}

#define STD_IN_BUFFER_SIZE 4096
char *stdin_uart0() {
	uint32_t *buffer = malloc(sizeof(uart.UARTDR) * STD_IN_BUFFER_SIZE);
	uint32_t *iterator = buffer;
	do {
		if(uart.UARTCR & TXE > 0) {
			break;
		} else {
			*iterator = uart.UARTDR;
			iterator++;
		}
	} while (*iterator != '\0');

	return buffer;
}
