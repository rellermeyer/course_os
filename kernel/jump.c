#include "include/jump.h"
#include "include/klibc.h"

void jmp_print(jmp_buf * buffer) {
    DEBUG("r0[0x%X]\n", buffer->R0);
    DEBUG("r1[0x%X]\n", buffer->R1);
    DEBUG("r2[0x%X]\n", buffer->R2);
    DEBUG("r3[0x%X]\n", buffer->R3);
    DEBUG("r4[0x%X]\n", buffer->R4);
    DEBUG("r5[0x%X]\n", buffer->R5);
    DEBUG("r6[0x%X]\n", buffer->R6);
    DEBUG("r7[0x%X]\n", buffer->R7);
    DEBUG("r8[0x%X]\n", buffer->R8);
    DEBUG("r9[0x%X]\n", buffer->R9);
    DEBUG("r10[0x%X]\n", buffer->R10);
    DEBUG("r11[0x%X]\n", buffer->R11);
    DEBUG("r12[0x%X]\n", buffer->R12);
    DEBUG("r13[0x%X]\n", buffer->R13);
    DEBUG("r14[0x%X]\n", buffer->R14);
    DEBUG("r15[0x%X]\n", buffer->R15);
}
