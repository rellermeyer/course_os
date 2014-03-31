/* Worked on by Jeremy Wenzel and Sam Allen */
#include "elf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

unsigned char* filePointer;

/* Gets value of bytes given a size of bytes */
int do_big_endian(int size) {
	unsigned int value = 0;
	while(size > 0) {
		value = value << 8;
		value = value + *filePointer;
		filePointer++;
		size--;
	}
	return value;
}

int do_little_endian(int size) {
	unsigned int value = 0;
	unsigned int num = 0;
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

int get_value(int size, Elf_Ehdr h)
{
	unsigned int ret;
	if(h.e_ident[EI_DATA] == 1)
		ret = do_little_endian(size);
	else
		ret = do_big_endian(size);
	return ret;
}

int isElf(Elf_Ehdr h) {
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

void read_elf_header(Elf_Ehdr h) {
	int check = isElf(h);
	if(check == -1)
		return;
	
	h.e_ident[EI_CLASS] = get_value(1, h);		// get class

	h.e_ident[EI_DATA] = get_value(1, h);		// endian
	h.e_ident[EI_VERSION] = get_value(1, h);	// original version of ELF
	h.e_ident[EI_OSABI] = get_value(1, h);		// Target operating system ABI
	h.e_ident[EI_ABIVERSION] = get_value(1, h); 	// Don't really know?
	int skip = 0;					// skips the padding
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

}

/* Don't use this unless we have printf functionality */
void print_elf_header(Elf_Ehdr h) {
	int class, endian, version, osabi, abiVersion;
	class = h.e_ident[EI_CLASS];
	endian = h.e_ident[EI_DATA];
	version = h.e_ident[EI_VERSION];
	osabi = h.e_ident[EI_OSABI];
	abiVersion = h.e_ident[EI_ABIVERSION];

	// class variable
	if (class == 1)
	{
		printf("Class: 32-bit objects\n");
	}
	else if (class == 2)
	{
		printf("Class: 64-bit objects\n");
	}
	else //invalid
	{
		printf("Invalid class: %i.\n", class);
	}
	
	// data/endian variable
	if (endian == 1)
	{
		printf("Data Format: Little Endian\n");
	}
	else if (endian == 2)
	{
		printf("Data Format: Big Endian\n");
	}
	else //invalid
	{
		printf("Invalid data encoding: %i.\n", endian);
	}	

	// version variable
	if (version == 1)
	{
		printf("Version: Currrent version\n");
	}
	else	//invalid
	{
		printf("Invalid version: %i.\n", version);
	}

	// OS ABI variable
	if (osabi == 0)
	{
		printf("OS-specific flags: None\n");
	}
	else	//??? not sure what to do otherwise
	{
		printf("OS ABI value: %i\n", osabi);
	}

	// ABI version variable
	if (abiVersion == 0)
	{
		//don't worry about it
	}
	else	//not sure what to do - probably doesn't matter
	{
		printf("Invalid ABI version: %i.\n", abiVersion);
	}
	
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
	read_elf_header(e);

	free(file_contents);
	return 0;
}
