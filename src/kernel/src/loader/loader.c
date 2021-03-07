#include <loader.h>
#include <stdio.h>
#include <vas2.h>

int loadProcessFromElfFile(void * PCB, void * file) {

	// validate the input
	if(PCB == NULL || file == NULL) return -1;

	// Parse the header
	Elf elf_info;
	Elf32_Header * header = (Elf32_Header *)file;
	int result = elf_parse_header(&elf_info, header);
    if (result == -1) return -1;

    // Get the pointers to the header tables
	Elf32_SectionHeader * section_header_table = file + elf_info.sectionHeaderTableOffset;
	Elf32_ProgramHeader * program_header_table = file + elf_info.programHeaderTableOffset;

    // Initialize a new address space for the new process to create
    struct vas2 * new_vas = create_vas();

    // Process the program(segment) header table - allocate pages, copy the loadable segments
    processProgramHeaderTable(new_vas, file, program_header_table, elf_info.programHeaderTableLength);

    // Add the pointer to the new address space to the PCB

	return 0;
}

int processProgramHeaderTable(struct vas2 * vasToFill, void * file, Elf32_ProgramHeader * phtable, Elf32_Word t_size) {

    // Iterate over the program headers
    for (Elf32_Word ph_index = 0; ph_index < t_size; ph_index++) {

        // Get the current program header
        Elf32_ProgramHeader * currentProgramHeader = phtable + ph_index;

        if (!validate_program_header(currentProgramHeader)) {
            return -1;
        }

        if (currentProgramHeader->program_type == EPT_LOAD) {

            // Get the virtual address
            Elf32_Addr virtualAddress = currentProgramHeader->program_virtual_addr;
            Elf32_Addr programToRead =
                (Elf32_Addr)((char *)file + currentProgramHeader->program_offset);
            Elf32_Word fileSize = currentProgramHeader->program_filesz;
            Elf32_Word sizeLeftToAllocate = currentProgramHeader->program_memsz;

            // While there is more memory space that needs to be allocated
            while (sizeLeftToAllocate > 0) {

                    allocate_page(vasToFill,virtualAddress, currentProgramHeader->flags & EPF_EXEC);
                    sizeLeftToAllocate -= PAGE_SIZE;
                    virtualAddress += PAGE_SIZE;

            }

            // TODO copy the segment data into the newly allocated pages

        }

        /* TODO Allocate pages for the stack and the heap.
        *  STACK
        *   ||  - // TODO MB ?
        *   \/
        *  -----
        *   /\
        *   ||  - // TODO MB ?
        *  HEAP
         */

    }

    return 0;

}

int printSectionNames(void * file, Elf32_SectionHeader * shtable, Elf32_Word t_size, Elf32_Half names_index) {

	Elf32_SectionHeader * names_section = shtable + names_index;

	Elf32_Addr names = (uint32_t)((char *)file + names_section->section_offset);
	
	Elf32_Word size = names_section->section_size;

	Elf32_Word i = 0;

	while(i < t_size) {
		
		// Do not make boundary checks for the offset into the section name table
		kprintf("%s \n", (char *)names + shtable[i].section_name);
        i++;
	}

	return 0;
}