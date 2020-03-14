#include <stdio.h>

void kernel_one() {
    /** asm volatile(
        "mov %r7, $4 \n" \
        "swi $0 \n"
    ); */
    kprintf("Starting kernel program one.\n");

    for (size_t i = 0; i < 1000; i++) {
        kprintf("Hello #%i from program one.\n", i);
    }

    kprintf("Goodbye from program one.\n");
}

void kernel_two() {
    asm volatile(
        "mov %r7, $4 \n" \
        "swi $0 \n"
    );
    kprintf("Starting kernel program two.\n");

    for (size_t i = 0; i < 1000; i++) {
        kprintf("Hello #%i from program two.\n", i);
    }

    kprintf("Goodbye from program two.\n");
}
