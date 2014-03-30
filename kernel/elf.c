/* Worked on by Jeremy Wenzel and Sam Allen */
#include "elf.h"
#include <stdio.h>

int isElf(FILE *f, Elf_Ehdr h) {
	// checks if elf file
	char ELFMAG0 = fgetc(f);	// 0x7f	
	char ELFMAG1 = fgetc(f);	// 'E'
	char ELFMAG2 = fgetc(f);	// 'L'
	char ELFMAG3 = fgetc(f);	// 'F'
	if(ELFMAG0 != 127 || ELFMAG1 != 'E' || ELFMAG2 != 'L' || ELFMAG3 != 'F')
		return -1;
	h.e_ident[EI_MAG0] = ELFMAG1;
	h.e_ident[EI_MAG1] = ELFMAG2;
	h.e_ident[EI_MAG2] = ELFMAG3;
	return 1;
}

void read_elf_header(FILE *f, Elf_Ehdr h) {
	int check = isElf(f, h);
	if(check == -1)
		printf("File is not an ELF file");
	else
		printf("File IS an ELF file");

	h.e_ident[EI_CLASS] = fgetc(f);		// get class
	h.e_ident[EI_DATA] = fgetc(f);		// endian
	h.e_ident[EI_VERSION] = fgetc(f);	// original version of ELF
	h.e_ident[EI_OSABI] = fgetc(f);		// Target operating system ABI
	h.e_ident[EI_ABIVERSION] = fgetc(f); 	// 
	int skip = 0;				// skips the padding
	while(skip < 7) {
		fgetc(f);
		skip++;
	}
	char e_type = fgetc(f);			// get type of file
	printf("type is %i\n", ((int)e_type));
	
	// get
}

void print_elf_header(Elf_Ehdr h) {
	// prints the information of the header file
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

int main(int argc, char *argv[]) {
	char *arg = *argv;
	FILE *input = fopen(arg, "r");
	if(input == NULL) {
		printf("File did not open; exiting");
		return(-1);
	}
	Elf_Ehdr h;
	read_elf_header(input, h);
	
	
	return 0;
	
}
