#include "include/loader.h"
#include "include/mem_alloc.h"
#include "include/klibc.h"
#include "include/process.h"
#include <stdint.h>
#include "include/elf.h"
#define USER_PROC_STACK_SIZE 0x100000 //1 MB
#define KERNEL_PROC_STACK_SIZE 0x1000 //4k

void det_mem_block_size(Elf_Ehdr *h, Elf_Shdr *sh[], uint32_t * file_pointer)
{

}

//Probably want to return a memory address rather than nothing.
//Take a process control block and pointer to the start of an ELF file in memory.
void load_file(pcb * process_control_block, uint32_t * file_pointer)
{
	Elf_Ehdr *h = (Elf_Ehdr *)kmalloc(sizeof(Elf_Ehdr));
	read_elf_header(*h, file_pointer);
	
	Elf_Phdr ph[h->e_phnum];
	read_program_header_table(*h, ph, file_pointer);

	Elf_Shdr sh[h->e_shnum];
	read_section_header_table(*h, sh, file_pointer);
	
	parse_section_header_names(*h, sh, file_pointer);	
}

//this will allocate memory for the entire process
//including the pcb 
void allocate_process_memory(pcb* pcb_p)
{
	//determine sizes for each section in ELF
	//sum dem digitz
	//determine which stack size to use	
}
