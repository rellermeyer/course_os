/*
 * setjmp.h
 *
 *  Created on: May 2, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_JUMP_H_
#define KERNEL_INCLUDE_JUMP_H_

#include <global_defs.h>

typedef struct jmp_buf {
	uint32_t R0;
	uint32_t R1;
	uint32_t R2;
	uint32_t R3;
	uint32_t R4;
	uint32_t R5;
	uint32_t R6;
	uint32_t R7;
	uint32_t R8;
	uint32_t R9;
	uint32_t R10;
	uint32_t R11;
	uint32_t R12;
	uint32_t R13;
	uint32_t R14;
	uint32_t R15;
} jmp_buf;

uint32_t jmp_set(jmp_buf * buffer);
uint32_t jmp_goto(jmp_buf * buffer, uint32_t value);
void jmp_print(jmp_buf * buffer);

#endif /* KERNEL_INCLUDE_JUMP_H_ */
