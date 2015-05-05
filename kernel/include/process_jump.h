/*
 * setjmp.h
 *
 *  Created on: May 2, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_PROCESS_JUMP_H_
#define KERNEL_INCLUDE_PROCESS_JUMP_H_

#include <global_defs.h>
#include "jump.h"

uint32_t process_jmp_set(jmp_buf * buffer);
uint32_t process_jmp_goto(jmp_buf * buffer,
		uint32_t value);
uint32_t proces_enter_umode(uint32_t lr);

#endif /* KERNEL_INCLUDE_JUMP_H_ */
