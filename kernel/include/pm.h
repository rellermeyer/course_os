#ifndef _PM_H
#define _PM_H
#include "klibc.h"
#include "global_defs.h"
#include "stdint.h"
#include "mmap.h"

typedef struct
{

	//CPU peripherals
	uint32_t PIC;
	uint32_t VFP;
	uint32_t PMU;

	//registers
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
} cpu_state;
