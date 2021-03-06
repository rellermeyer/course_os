#include <loader.h>
#include <stdio.h>

int loadProcessFromElfFile(void * PCB, void * file) {

	// validate the input
	if(PCB == NULL || file == NULL) return -1;

	// Parse the header
	Elf elf_info;

	Elf32_Header * header = (Elf32_Header *)file;
	
	int result = elf_parse_header(&elf_info, header);
	
	Elf32_SectionHeader * section_header_table = file + elf_info.sectionHeaderTableOffset;
	Elf32_ProgramHeader * program_header_table = file + elf_info.programHeaderTableOffset;

	int res_printing = printSectionNames(file, section_header_table, elf_info.sectionHeaderTableLength, elf_info.sht_index_names);

	
		
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

	}

	return 0;
}
