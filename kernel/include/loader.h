#ifndef __loader_h
#define __loader_h
#include "global_defs.h"
#include "elf.h"
#include "process.h"

#define USER_PROC_STACK_SIZE 0x100000 //1 MB
#define KERNEL_PROC_STACK_SIZE 0x1000 //4K
#define USER_PROC_HEAP_SIZE 4096
#define PADDING 0x1000

Elf_Ehdr* load_file(pcb *, uint32_t *); //Needs to be of the type that's where ever our beginning of file is in mem

#endif
