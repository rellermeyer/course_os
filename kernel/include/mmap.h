/*
 *  Memory Mapped I/O addresses 
 */

#ifndef __MMAP_H__
#define __MMAP_H__

#define mmio_read(address)		(*((volatile uint32_t *)(address)))
#define mmio_write(address, value)	(*((volatile uint32_t *)(address)) = (volatile uint32_t)(value))

#define PIC_ADDRESS	(volatile uint32_t *const) 0x10140000	/* interrupt controller peripheral */
#define UART_ADDRESS	(volatile uint32_t *const) 0x101f1000	/* UART 0 base address */

#endif // __MMAP_H_
