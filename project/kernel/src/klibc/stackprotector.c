#include <stdint.h>
#include <klibc.h>

#define STACK_CHK_GUARD 0x42069

intptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail(void) {
    kprintf("%s\n", "Stack was smashed killing Tweety!");
    panic();
}
