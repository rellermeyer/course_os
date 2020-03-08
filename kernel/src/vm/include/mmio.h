#ifndef MMIO_H
#define MMIO_H


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

#endif

