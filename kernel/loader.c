#include "include/loader.h"
#include "include/mem_alloc.h"
#include "include/klibc.h"
#include "include/process.h"
#include <stdint.h>
#include "include/elf.h"

//We determine what the size of our process is going to be
//Determine the size of the segments that need to be loaded in (has p_type == PT_LOAD)
//Account for the other stuff we'll need (Stack, Heap, spacing)
size_t det_proc_size(Elf_Ehdr *h, Elf_Phdr ph[])
{
	size_t process_size = 0;
	int i = 0;
	//Add up the size of all the necessary sections that 
	for(; i < (h->e_phnum); i++)
	{
		if(ph[i].p_type == PT_LOAD)
		{
			process_size += ph[i].p_memsz;
		}
	}
	
	process_size +=  USER_PROC_STACK_SIZE;
	process_size += 0x10000; //This is going to be the heap size (need to determine what this should be
	
	 //Padding we want to be able to have some padding between certain sections of the program 
	//mainly the stack from the other parts
	process_size += 0x1000;
	
	return process_size;
	
	
}

//Temporary add this to somewhere relevant (klibc)
void memcpy(uint32_t * source, uint32_t * dest, size_t size)
{
	size_t i = 0;
	for(; i < size; i++)
	{
		*(source + size) = *(dest + size);
	}
}


void allocate_process_memory(pcb *pcb_p, Elf_Ehdr *h, Elf_Phdr ph[], uint32_t * file_pointer)
{
	 size_t process_size = det_proc_size(h, ph);
	uint32_t * process_mem = u_malloc(process_size);
	uint32_t * current_pointer = process_mem;
	//We're gonna copy each segment into memory at a position. Calculate what needs to done to change
	//the entry_point to account for where we've placed something in memory
	
	//memcpy(process_mem, pointer to read only segment, size of RO Segment);
	//memcpy(process_mem + size of RO Segment, pointer to Read and write segment, size of R&W segment)	
	//make a pointer to heap process_mem + size of RO segment + size of R&W segment + maybe some padding
	//make a pointer to stack around the top of our allocated memory
	/* TOP | STACK |
	*      | ***** |
	*      | ***** | } size = USER_PROC_STACK_SIZE 
	*      | ***** | Grows down
	*      | ***** |
	*      | ***** |
	*      | *Pad* | //Separates stack from others so no overwriting is done
	*      | ***** |
        *      | ***** |
        *      | ***** | } size = HEAP_SIZE NEED TO DECIDE
	*      | Heap  |
	*      | *Pad* |
	*      |  R&W  |
	*      |  Seg  |
	*      | ***** |
	*      |  RO   |
	*      |  Seg  |
	*      | ***** | <- process_mem	
	*/
	//Puts all the relevant segments into allocated region
	int i = 0;
	for(; i < (h->e_phnum); i++)
        {
		if(ph[i].p_type == PT_LOAD)
		{
			memcpy(file_pointer + ph[i].p_offset, current_pointer, ph[i].p_memsz);
			current_pointer = current_pointer + ph[i].p_memsz;
		}
	}

	
	
			
		
}

//Probably want to return a memory address rather than nothing.
//Take a process control block and pointer to the start of an ELF file in memory.
void load_file(pcb * process_control_block, uint32_t * file_pointer)
{
	Elf_Ehdr *h = (Elf_Ehdr *)k_malloc(sizeof(Elf_Ehdr));
	read_elf_header(*h, file_pointer);
	
	Elf_Phdr ph[h->e_phnum];
	read_program_header_table(*h, ph, file_pointer);

	//Elf_Shdr sh[h->e_shnum];
	//read_section_header_table(*h, sh, file_pointer);
	
	//parse_section_header_names(*h, sh, file_pointer);	
}

//this will allocate memory for the entire process
//including the pcb 
//void allocate_process_memory(pcb* pcb_p)
//{
	//determine sizes for each section in ELF
	//sum dem digitz
	//determine which stack size to use	
//	size_t process_mem = det_proc_size(
//}
