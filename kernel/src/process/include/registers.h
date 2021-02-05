#ifndef COURSE_OS_REGISTERS_H
#define COURSE_OS_REGISTERS_H

#include <stdint.h>

typedef struct Registers {
    size_t R0;
    size_t R1;
    size_t R2;
    size_t R3;
    size_t R4;
    size_t R5;
    size_t R6;
    union {
        size_t R7;
        size_t WR;
    };
    size_t R8;
    union {
        size_t R9;
        size_t SB;
    };
    union {
        size_t R10;
        size_t SL;
    };
    union {
        size_t R11;
        size_t FP;
    };
    union {
        size_t R12;
        size_t IP;
    };
    union {
        size_t R13;
        size_t SP;
    };
    union {
        size_t R14;
        size_t LR;
    };
    union {
        size_t R15;
        size_t PC;
    };
    size_t CPSR;
} Registers;

#endif //COURSE_OS_REGISTERS_H
