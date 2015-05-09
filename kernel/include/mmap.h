/*
 *  Memory Mapped I/O addresses 
 */

#ifndef __MMAP_H__
#define __MMAP_H__

#define mmio_read(address)		(*((volatile uint32_t *)(address)))
#define mmio_write(address, value)	(*((volatile uint32_t *)(address)) = (volatile uint32_t)(value))

void mmap(void *p_bootargs);

#define PIC_ADDRESS	((volatile char *const) 0x10140000)	/* interrupt controller peripheral */
#define UART0_ADDRESS	(volatile uint32_t *const) 0x101f1000	/* UART 0 base address */
#define CLOCK_ADDRESS	(volatile uint32_t *const) 0x101e8000	/* RTC base address */

/* Physical Hardware Addresses */
#define PERIPHERAL_BASE_PI	(volatile uint32_t *const) 0x20000000
#define PIC_ADDRESS_PI		(volatile uint32_t *const) 0x2000B000	/* interrupt controller peripheral */
#define UART0_ADDRESS_PI	(volatile uint32_t *const) 0x20201000	/* UART 0 base address */
#define CLOCK_ADDRESS_PI	(volatile uint32_t *const) 0x20003000	/* RTC base address */
#define GPIO_ADDRESS_PI		(volatile uint32_t *const) 0x20200000

#endif // __MMAP_H_
