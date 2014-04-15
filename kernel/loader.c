#include "include/loader.h"
#include "include/mem_alloc.h"
#include "include/klibc.h"
#include "include/process.h"
#include <stdint.h>

void det_mem_block_size(Elf_Ehdr *h, Elf_Shdr *sh[], uint32_t * file_pointer)
{

}

//Probably want to return a memory address rather than nothing.
//Take a process control block and pointer to the start of an ELF file in memory.

void load_file(pcb * process_control_block, uint32_t * file_pointer)
{
	Elf_Ehdr *h = (Elf_Ehdr *)mem_alloc(sizeof(Elf_ehdr));
	read_elf_header(*h, file_pointer);
	
	Elf_Phdr ph[h->e_phnum];
	read_program_header_table(h, ph);

	Elf_Shdr sh[h->e_shnum];
	read_section_header_table(h, sh);
	
	parse_section_header_names(h, sh, file_pointer);	
}

