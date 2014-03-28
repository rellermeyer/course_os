/* Worked on by Jeremy Wenzel and Sam Allen */
#include "elf.h"

bool isElf(Elf_Ehdr h) {
	// checks if elf file
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

void print_symbols(int32_t, Elf_Ehdr eh, Elf_Shdr sh_table[]) {
	// print symbols
}
