#ifndef COURSE_OS_INTERRUPT_ASM_H
#define COURSE_OS_INTERRUPT_ASM_H

#include "interrupt.h"

long __attribute__((interrupt("SWI"))) software_interrupt_handler();  // 0x08

#endif  // COURSE_OS_INTERRUPT_ASM_H
