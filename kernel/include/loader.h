#ifndef __loader_h
#define __loader_h
#include "global_defs.h"
#include "elf.h"
#include "process.h"

//typedef enum{TEXT, RODATA, DATA, RODATA, BSS, SYMTBL} Section;

void load_file(pcb *, uint32_t *); //Needs to be of the type that's where ever our beginning of file is in mem

#endif
