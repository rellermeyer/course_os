#include <stdint.h>
#include "drivers/uart.h"
#include "mmap.h" 

void print_uart0(const char *s)
{
	while (*s != '\0')
	{
		mmio_write(UART0_ADDRESS, *s);
		s++;
	}
}

void print_char_uart0(char c)
{
	mmio_write(UART0_ADDRESS, c);
}

/* print the full 32 bits of a word at the given address */
/* trailing newline... */
void print_word_bits(uint32_t * c)
{
	int i;
	for (i = 31; i >= 0; i--)
		*c & (1 << i) ? print_uart0("1") : print_uart0("0");
	print_uart0("\n");
}

/* print the full 8-digit hex code of a word at the given address */
/* no '0x' prefix, NO trailing newline */
void print_word_hex(uint32_t * c)
{
	int i;
	uint32_t a;
	for (i = 0x7; i >= 0x0; i--)
	{
		a = *c & (0xf << (i * 0x4));
		a >>= (i * 0x4);

		if (a <= 9)
			mmio_write(UART0_ADDRESS, (uint32_t )(a + (uint32_t )'0'));
		else if (a <= 0xf)
			mmio_write(UART0_ADDRESS, (uint32_t )((a - 0xa) + (uint32_t )'a'));
		else
			mmio_write(UART0_ADDRESS, (uint32_t )('?'));
	}
}

/* display memory at given address */
/* format is: "[address]: word1 word2 word3\n", etc. */
/* displays 30 words (10 lines) */
void md(uint32_t * start)
{
	int i, j;
	uint32_t *addr = start;
	for (i = 0; i < 10; i++)
	{
		print_uart0("0x");
		print_word_hex((uint32_t *) &addr);
		print_uart0(": ");
		for (j = 0; j < 3; j++)
		{
			print_word_hex(addr);
			print_uart0("  ");
			addr++;
		}
		print_uart0("\n");
	}
}

/*void print_uart0(const char *s) {*/
/*while (uart->dd->uart0_inter_val == 0) {}*/
/*if(uart.UARTCR & RXE > 0) */
/*{*/
/*while(*s != '\0') */
/*{*/
/**UART0 = (uint32_t)(*s);*/
/*s++;*/
/*}*/
/*}*/
/*}*/

/*  We need to implement a lock here.  klibc will be implementing the buffer
 *    we just need to ensure the FIFO isn't read out of order.
 */
/*char *read_uart0() */
/*{*/
/*uint32_t buffer[STD_IN_BUFFER_SIZE] = {0};*/
/*while (uart->dd->uart0_inter_val == 0) {}*/
/*uint32_t *iterator = buffer;*/
/*do */
/*{*/
/*if(uart.UARTCR & TXE > 0) */
/*{*/
/*break;*/
/*} */
/*else */
/*{*/
/**iterator = uart.UARTDR;*/
/*iterator++;*/
/*}*/
/*} while (*iterator != '\0');*/
/*return buffer;*/
/*}*/

