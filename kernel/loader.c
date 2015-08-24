#include <stdint.h>
#include "loader.h"
#include "mem_alloc.h"
#include "klibc.h"
#include "process.h"
#include "elf.h"

//Worked on by Kaelen Haag and Jeremy Wenzel
//We determine what the size of our process is going to be
//Determine the size of the segments that need to be loaded in (has p_type == PT_LOAD)
//Account for the other stuff we'll need (Stack, Heap, spacing)
os_size_t det_proc_size(Elf_Ehdr *h, Elf_Phdr ph[])
{
	os_size_t process_size = 0;
	int i = 0;
	//Add up the size of all the necessary sections that 
	for (; i < (h->e_phnum); i++)
	{
		if (ph[i].p_type == PT_LOAD)
		{
			process_size += ph[i].p_memsz;
		}
	}

	process_size += USER_PROC_STACK_SIZE;
	process_size += USER_PROC_HEAP_SIZE; //This is going to be the heap size (need to determine what this should be

	//Padding we want to be able to have some padding between certain sections of the program 
	//mainly the stack from the other parts
	process_size += PADDING;

	return process_size;

}

//Probably want to return a memory address rather than nothing.
//Take a process control block and pointer to the start of an ELF file in memory.
Elf_Ehdr* load_file(pcb * pcb_p, uint32_t * file_pointer)
{
	Elf_Ehdr *h = (Elf_Ehdr *) kmalloc(sizeof(Elf_Ehdr)); // Get elf header
	os_printf("elf header= %x\n", h);
	int i = read_elf_header(h, (unsigned char *) file_pointer);

	if (i == -1)
	{
		os_printf("File is Not an ELF File. Exiting\n");
		return 0;
	}

	if (h->e_phnum == 0)
	{
		os_printf("No Program headers in ELF file. Exiting\n");
		return 0;
	}

	Elf_Phdr * ph = (Elf_Phdr *) kmalloc(h->e_phnum * sizeof(Elf_Phdr));
	Elf_Shdr * sh = (Elf_Shdr *) kmalloc(h->e_shnum * sizeof(Elf_Shdr));
	// FIXME: why is sh not used?
	UNUSED(sh);
	read_program_header_table(h, ph, (unsigned char *) file_pointer);

	//allocate pages for the read only segment

	//allocate pages for text section

	//assert(1 == 2 && "load_file");

	//read_section_header_table(h, sh, (uint32_t*)file_pointer);
	// /vm_enable_vas(pcb_p->stored_vas);

	//allocate_process_memory(pcb_p, h, ph, file_pointer);

	//vm_use_kernel_vas();
	return h;
}

