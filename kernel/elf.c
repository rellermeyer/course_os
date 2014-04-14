/* Worked on by Jeremy Wenzel and Sam Allen */
#include "include/elf.h"
#include "include/libc.h"
#include <stdint.h>		// Probably going to be removed


uint32_t* filePointer;
uint32_t* startPointer;

/* Gets the value of the bytes on a big endian system */
uint32_t do_big_endian(uint32_t size) {
	uint32_t value = 0;
	while(size > 0) {
		value = value << 8;
		value = value + *filePointer;
		filePointer++;
		size--;
	}
	return value;
}

/* Gets the value of the bytes on a little endian system */
uint32_t do_little_endian(uint32_t size) {
	uint32_t value = 0;
	uint32_t num = 0;
	int i = 0;
	while(i < size) {
		num = *filePointer;
		num = num << (8 * i);
		value = value + num;
		filePointer++;
		i++;
	}
	return value;

}

/* Gets value of bytes given a size of bytes */
uint32_t get_value(uint32_t size, Elf_Ehdr h)
{
	if(h.e_ident[EI_DATA] == 1)
		return do_little_endian(size);
	else
		return do_big_endian(size);
}

/* Checks if the file is even an elf file */
/* Returns 1 if elf file, -1 if not */
int32_t isElf(Elf_Ehdr h) {
	// checks if elf file
	char ELFMAG0 = get_value(1, h);	// 0x7f
	char ELFMAG1 = get_value(1, h);	// 'E'
	char ELFMAG2 = get_value(1, h);	// 'L'
	char ELFMAG3 = get_value(1, h);	// 'F'
	if(ELFMAG0 != 127 || ELFMAG1 != 'E' || ELFMAG2 != 'L' || ELFMAG3 != 'F')
		return -1;
	h.e_ident[EI_MAG0] = ELFMAG1;
	h.e_ident[EI_MAG1] = ELFMAG2;
	h.e_ident[EI_MAG2] = ELFMAG3;
	return 1;
}

/* Parses elf header of ELF file and places into ELF struct header */
/* CRITICAL: It is important to initialize the elf header before
	sending it into the function. Makes sure no data is lost */
int read_elf_header(Elf_Ehdr h, uint32_t *pointer) {
	filePointer = startPointer = pointer;
	int32_t check = isElf(h);
	if(check == -1){
		return -1;
	}
	
	
	h.e_ident[EI_CLASS] = get_value(1, h);		// get class
	h.e_ident[EI_DATA] = get_value(1, h);		// endian
	h.e_ident[EI_VERSION] = get_value(1, h);	// original version of ELF
	h.e_ident[EI_OSABI] = get_value(1, h);		// Target operating system ABI
	h.e_ident[EI_ABIVERSION] = get_value(1, h); 	// Don't really know?
	
	int32_t skip = 0;				// skips the padding
	while(skip < 7) {
		get_value(1, h);
		skip++;
	}
	
	h.e_type = get_value(2, h);			// get type of file
	h.e_machine = get_value(2, h);			// gets machine type (should be arm in our case)
	h.e_version = get_value(4, h);			// version number (should be 1)
	
	if(h.e_ident[EI_CLASS] == 1)			// get entry point
		h.e_entry = get_value(4, h);
	else
		h.e_entry = get_value(8, h);
	
	if(h.e_ident[EI_CLASS] == 1)			// get program header offset
		h.e_phoff = get_value(4, h);
	else
		h.e_phoff = get_value(8, h);
	
	if(h.e_ident[EI_CLASS] == 1)			// get section header offset
		h.e_shoff = get_value(4, h);
	else
		h.e_shoff = get_value(8, h);

	h.e_flags = get_value(4, h);			// get flag number
	h.e_ehsize = get_value(2, h);			// get elf header size
	h.e_phentsize = get_value(2, h);		// get program header size
	h.e_phnum = get_value(2, h);			// number of program headers
	h.e_shentsize = get_value(2, h);		// section header size
	h.e_shnum = get_value(2, h);			// number of section headers
	h.e_shstrndx = get_value(2, h);			// section header string table index


	return 1;	// return success
}


void read_program_header_table(Elf_Ehdr eh, Elf_Phdr ph[], uint32_t *pointer) {
	filePointer = startPointer = pointer;
	filePointer = startPointer + eh.e_phoff;
	int i = 0;
	while(i < eh.e_phnum) {
		ph[i].p_type = get_value(4, eh);
		ph[i].p_offset = get_value(4, eh);
		ph[i].p_vaddr = get_value(4, eh);
		ph[i].p_paddr = get_value(4, eh);
		ph[i].p_filesz = get_value(4, eh);
		ph[i].p_memsz = get_value(4, eh);
		ph[i].p_flags = get_value(4, eh);
		ph[i].p_align = get_value(4, eh);
		i++;
	}
}

/* Reads the section header table and places it into the the section header array */
/* CRITICAL: The section header must be initalized with the correct size,
	 before put into the function */
void read_section_header_table(Elf_Ehdr eh, Elf_Shdr sh[], uint32_t *pointer) {
	filePointer = startPointer = pointer;
	int i = 0;
	filePointer = startPointer + eh.e_shoff;	
	while(i < eh.e_shnum) {	
		sh[i].sh_name = get_value(4, eh);		// get name, which is index into string table
		sh[i].sh_type = get_value(4, eh);		// type of section header
		sh[i].sh_flags = get_value(4, eh);		// flags that partain to the header
		sh[i].sh_addr = get_value(4, eh);		// address of header (don't know much yet)
		sh[i].sh_offset = get_value(4, eh);		// offset from beginning of elf file
		sh[i].sh_size = get_value(4, eh);		// size of this section
		sh[i].sh_link = get_value(4, eh);		// link (don't know much yet)
		sh[i].sh_info = get_value(4, eh);		// info (don't know much yet)
		sh[i].sh_addralign = get_value(4, eh);		// address aligned (don't know much yet)
		sh[i].sh_entsize = get_value(4, eh);		
		i++;
	}
}

/* Goes to the Section header that contains the section header string table */
/* It then analyzes the table and puts them into each section header */
/* This makes it easier to differentiate between sections when put into loader */
void parse_section_header_names(Elf_Ehdr eh, Elf_Shdr sh[], uint32_t *pointer) {
	filePointer = startPointer = pointer;
	filePointer = startPointer + sh[eh.e_shstrndx].sh_offset;	// This pointer iterates through the section header
	int i = 1;
	
	filePointer++;
	while(i < eh.e_shnum) {
		if(os_strcmp(((char*)filePointer), ".symtab") == 0) {		// checks if symbol table
			sh[i].sh_numname = SYMTAB;
		}
		else if(os_strcmp(((char*)filePointer), ".text") == 0) {	// checks if .text table
			sh[i].sh_numname = TEXT;
		}
		else if(os_strcmp(((char*)filePointer), ".comment") == 0) {	// checks if comment table
			sh[i].sh_numname = COMMENT;
		}
		else if(os_strcmp(((char*)filePointer), ".strtab") == 0) {	// checks if string table
			sh[i].sh_numname = STRTAB;
		}
		else if(os_strcmp(((char*)filePointer), ".shstrtab") == 0) {	// checks if section header string table
			sh[i].sh_numname = SHSTRTAB;
		}
		else if(os_strcmp(((char*)filePointer), ".rodata") == 0) {	// checks if read only data
			sh[i].sh_numname = RODATA;
		}
		else if(os_strcmp(((char*)filePointer), ".data") == 0) {	// checks if initialized data
			sh[i].sh_numname = DATA;
		}
		else if(os_strcmp(((char*)filePointer), ".bss") == 0) {		// checks if unitialized data
			sh[i].sh_numname = BSS;
		}
		else;
		while(*((char*)filePointer) != '\0')		// move on to next sting in the list
			filePointer++;
		filePointer++;
		i++;	// move on to next item in section header table
	}
}
