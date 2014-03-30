/* Worked on by Jeremy Wenzel and Sam Allen */
#include "elf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char* filePointer;

void increment_pointer()
{
	filePointer+=1;
}
char get_next_char()
{
	increment_pointer();
	return *filePointer;
}

int isElf(Elf_Ehdr h) {
	// checks if elf file
	char ELFMAG0 = get_next_char();	// 0x7f	
	char ELFMAG1 = get_next_char();	// 'E'
	char ELFMAG2 = get_next_char();	// 'L'
	char ELFMAG3 = get_next_char();	// 'F'
	if(ELFMAG0 != 127 || ELFMAG1 != 'E' || ELFMAG2 != 'L' || ELFMAG3 != 'F')
		return -1;
	h.e_ident[EI_MAG0] = ELFMAG1;
	h.e_ident[EI_MAG1] = ELFMAG2;
	h.e_ident[EI_MAG2] = ELFMAG3;
	return 1;
}

void read_elf_header(Elf_Ehdr h) {
	int check = isElf(h);
	if(check == -1)
		printf("File is not an ELF file");
	else
		printf("File IS an ELF file");

	h.e_ident[EI_CLASS] = get_next_char();	// get class
	h.e_ident[EI_DATA] = get_next_char();	// endian
	h.e_ident[EI_VERSION] = get_next_char();// original version of ELF
	h.e_ident[EI_OSABI] = get_next_char();	// Target operating system ABI
	h.e_ident[EI_ABIVERSION] = get_next_char(); // 
	int skip = 0;				// skips the padding
	while(skip < 7) {
		get_next_char();
		skip++;
	}
	char e_type = get_next_char();			// get type of file
	printf("type is %i\n", ((int)e_type));
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
	file_contents = filePointer = read_whole_file("hello.c");
	printf("pointer %c\n", *filePointer);
	filePointer++;
	printf("%c\n", *filePointer);	
	char c = get_next_char();
	printf("%c\n", c);
	free(file_contents);
	return 0;
}
