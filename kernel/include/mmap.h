/*
 *  Memory Mapped I/O addresses 
 */

#define mmio_read(address)		(*(volatile u32_t *)(address))
#define mmio_write(address, value)	(*(volatile u32_t *)(address) = (value))

#define PIC_ADDRESS	0x10140000	/* interrupt controller peripheral */
#define UART_ADDRESS	0x101f1000	/* UART 0 base address */
