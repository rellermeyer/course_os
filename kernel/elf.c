/* Worked on by Jeremy Wenzel and Sam Allen */
#include "elf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

unsigned char* filePointer;

/* Gets value of bytes given a size of bytes */
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

uint32_t get_value(uint32_t size, Elf_Ehdr h)
{
	if(h.e_ident[EI_DATA] == 1)
		return do_little_endian(size);
	else
		return do_big_endian(size);
}

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

Elf_Ehdr read_elf_header(Elf_Ehdr h) {
	int32_t check = isElf(h);
	int i = 0;
	if(check == -1){
		printf("File is not an ELF file.\n");
		return h;
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
	h.e_machine = get_value(2, h);
	h.e_version = get_value(4, h);
	
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

	print_elf_header(h);
	while(i < h.e_phnum) {
		read_program_header_table(h);
		i++;
	}
	
	return h;
}

/* Don't use this unless we have printf functionality */
void print_elf_header(Elf_Ehdr h) {

	printf("ELF Header:\n");
	// class
	if (h.e_ident[EI_CLASS] == 1)
		printf("ELF32\n");
	else if (h.e_ident[EI_CLASS] == 2)
		printf("ELF64\n");
	else //invalid
		printf("Invalid class:			 %i\n", h.e_ident[EI_CLASS]);
	
	// data/endianness
	if (h.e_ident[EI_DATA] == 1)
		printf("Data Format: 				 Little Endian\n");
	else if (h.e_ident[EI_DATA] == 2)
		printf("Data Format: 				 Big Endian\n");
	else //invalid
		printf("Invalid data encoding:		 %i.\n", h.e_ident[EI_DATA]);

	// versio
	if (h.e_ident[EI_VERSION] == 1)
		printf("Version: 				 Currrent version\n");
	else	//invalid
		printf("Invalid version:		 %i.\n", h.e_ident[EI_VERSION]);

	// OS ABI
	if (h.e_ident[EI_OSABI] == 0)
		printf("OS-specific flags: 			 None\n");
	else	//??? not sure what to do otherwise
		printf("OS ABI value:			 %i\n", h.e_ident[EI_OSABI]);

	// ABI version
	if (h.e_ident[EI_ABIVERSION] != 0)
		printf("Invalid ABI version:		 %i.\n", h.e_ident[EI_ABIVERSION]);
	
	// File Type
	if (h.e_type == 1)
		printf("File type:			 	 Relocatable\n");
	else if (h.e_type == 2)
		printf("File type:			 	 Executable\n");
	else if (h.e_type == 3)
		printf("File type:				 Shared Object\n");
	else if (h.e_type == 4)
		printf("File type:			 	 Core\n");
	else
		printf("Invalid file type:		 	 %i\n", h.e_type);

	// Machine type
	if (h.e_machine == 40)
		printf("Machine architecture:			 ARM\n");
	else	
		printf("Invalid machine architecture.\n");

	// Born again version
	if (h.e_version != 1)
		printf("Invalid version.\n");

	// Entry point
	printf("Entry point:				 0x%08X\n", h.e_entry);

	// Program Header Offset
	printf("Program Header Offset:			 %i\n", h.e_phoff);

	// Section Header Offset
	printf("Section Header Offset:			 %i\n", h.e_shoff);

	// Flags
	if (h.e_flags == EF_ARM_ABIMASK)
		printf("EF_ARM_ABIMASK flag is set.\n");
	else if (h.e_flags == EF_ARM_BE8)
		printf("EF_ARM_BE8 flag is set.\n");
	else if (h.e_flags == EF_ARM_GCCMASK)
		printf("EF_ARM_GCCMASK flag is set.\n");
	else if (h.e_flags == EF_ARM_ABI_FLOAT_HARD)
		printf("EF_ARM_ABI_FLOAT_HARD flag is set.\n");
	else if (h.e_flags == EF_ARM_ABI_FLOAT_SOFT)
		printf("EF_ARM_ABI_FLOAT_SOFT flag is set.\n");
	else
		printf("No header flags set.\n");
	
	// Elf Header Size
	printf("Elf Header Size:			 %i\n", h.e_ehsize);

	// Program Header Size
	printf("Program Header Size:			 %i\n", h.e_phentsize);

	// Number of Program Headers
	printf("Number of Program Headers:		 %i\n", h.e_phnum);

	// Section Header Size
	printf("Section Header Size:			 %i\n", h.e_shentsize);

	// Number of Section Headers
	printf("Number of Section Headers:		 %i\n", h.e_shnum);

	// Section Header String Table Index
	printf("Section Header String Table Index:	 %i\n", h.e_shstrndx);

}

void print_program_header_table(Elf_Phdr ph) {
	printf("--------------------------------------\n");

	printf("Type: ");
	if(ph.p_type ==	PT_NULL)
		printf("NULL\n");
	else if(ph.p_type == PT_LOAD)
		printf("LOAD\n");
	else if(ph.p_type == PT_DYNAMIC)
		printf("DYNAMIC\n");
	else if(ph.p_type == PT_INTERP)
		printf("INTERP\n");
	else if(ph.p_type == PT_NOTE)
		printf("NOTE\n");
	else if(ph.p_type == PT_SHLIB)
		printf("SHLIB\n");
	else if(ph.p_type == PT_PHDR)
		printf("PHDR\n");
	else
		printf("Hdr not found... yet\n");

	
	printf("Offset: 	 	 0x%08X\n", ph.p_offset);
	printf("Virtual Address:	 0x%08X\n", ph.p_vaddr);
	printf("Physical address:	 0x%08X\n", ph.p_paddr);
	printf("File Size:		 0x%08X\n", ph.p_filesz);
	printf("MemSiz:			 0x%08X\n", ph.p_memsz);

	printf("Flags: 			 0x%08X\n", ph.p_flags);

	printf("Alignment:		 0x%08X\n", ph.p_align);
	printf("--------------------------------------\n");
}	

void read_program_header_table(Elf_Ehdr eh) {
	Elf_Phdr ph;

	ph.p_type = get_value(4, eh);
	ph.p_offset = get_value(4, eh);
	ph.p_vaddr = get_value(4, eh);
	ph.p_paddr = get_value(4, eh);
	ph.p_filesz = get_value(4, eh);
	ph.p_memsz = get_value(4, eh);
	
	ph.p_flags = get_value(4, eh);

	ph.p_align = get_value(4, eh);
	

	print_program_header_table(ph);
}
void read_section_header_table(int32_t fd, Elf_Ehdr h, Elf_Shdr sh_table[]) {
	// reads section header
}

void print_section_headers(int32_t fd, Elf_Ehdr h, Elf_Shdr sh_table[]) {
	// print the section header
} 

void print_symbol_table(int32_t fd, 
			Elf_Ehdr h, 
			Elf_Shdr sh_table, 
			uint32_t symbol_table)
{
	// prints the symbol table
}

void print_symbols(int32_t fd, Elf_Ehdr eh, Elf_Shdr sh_table[]) {
	// print symbols
}

unsigned get_file_size(char *file_name) {
	struct stat sb;
	if(stat(file_name, & sb) != 0) {
		fprintf(stderr, "Stat failed for file");
		exit(-1);
	}
	return sb.st_size;
}

unsigned char* read_whole_file(char *file_name) {
	unsigned s;
    unsigned char * contents;
    FILE * f;
    size_t bytes_read;
    int status;

    s = get_file_size (file_name);
    contents = malloc (s + 1);
    if (contents == NULL) {
        fprintf (stderr, "Not enough memory.\n");
        exit(-1);
    }

    f = fopen (file_name, "r");
    if (f == NULL) {
        fprintf (stderr, "Could not open File");
        exit (-1);
    }
    bytes_read = fread (contents, sizeof (unsigned char), s, f);
    if (bytes_read != s) {
        fprintf (stderr, "Short read of ");
        exit (-1);
    }
    status = fclose (f);
    if (status != 0) {
        fprintf (stderr, "Error closing ");
        exit (-1);
    }
    return contents;
}

int main() {
	unsigned char* file_contents;
	//char* pointer;
	file_contents = filePointer = read_whole_file("a.out");
	Elf_Ehdr e;
	//read_elf_header(e);
	e = read_elf_header(e);
	//print_elf_header(e);
	free(file_contents);
	return 0;
}
