/*
 *  Memory Mapped I/O addresses 
 */

#ifndef MMAP_H
#define MMAP_H

#include <stdint.h>

#define mmio_read(address) (*((volatile uint32_t *)(address)))
#define mmio_write(address, value) mmio_write_internal((volatile uint32_t *)(address), (volatile uint32_t)(value))

// Do the mmio write in assembly to remove any chance of gcc optimizing the call away. *yes* even if it was marked volatile already.
// Hence the 4 hours it took us to make timer.c work......
static inline void mmio_write_internal(volatile uint32_t * address, volatile uint32_t value ) {
    asm volatile (
        "ldr r1, %1\n"
        "str r1, %0\n"
        ::
        "m" (*address),
        "m" (value)
    );
}

void prepare_pagetable();
void mmap(void *p_bootargs);
void request_identity_mapped_section(size_t start_address, size_t megabytes);

#define CLOCK_ADDRESS	(volatile uint32_t *const) 0x101e8000	/* RTC base address */

/* Physical Hardware Addresses */
#define PERIPHERAL_BASE_PI	(volatile uint32_t *const) 0x20000000
#define PIC_ADDRESS_PI		(volatile uint32_t *const) 0x2000B000	/* interrupt controller peripheral */
//#define UART0_ADDRESS_PI	(volatile uint32_t *const) 0x20201000	/* UART 0 base address */
#define CLOCK_ADDRESS_PI	(volatile uint32_t *const) 0x20003000	/* RTC base address */
#define GPIO_ADDRESS_PI		(volatile uint32_t *const) 0x20200000

#endif // MMAP_H
