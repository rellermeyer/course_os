/* Any confusion with what does what can be gotten from the 
   following website. It defines all the values
	http://man7.org/linux/man-pages/man5/elf.5.html
*/

#ifndef _ELF_H_
#define _ELF_H_

#include <stdint.h>
#include "klibc.h"

typedef uint32_t Elf_Addr;	// Program Address
typedef uint16_t Elf_Half;	// 16 bit 
typedef uint32_t Elf_Off;	// File Offset
typedef int32_t Elf_Sword;	// Signed 32 bit int
typedef uint32_t Elf_Word;	// Unsigned 32 bit int
#define EI_NIDENT	16

/* Elf Header */
/* Note: The ELF Header should begin the following header
	EI_MAG0 = 0x7f
	EI_MAG1 = 'E'
	EI_MAG2 = 'L'
	EI_MAG3 = 'F'
*/

/* Magic Numbers and preliminary info about file */
#define EI_MAG0 	0	// 0x7f
#define EI_MAG1		1	// 'E'
#define EI_MAG2		2	// 'L'
#define EI_MAG3		3	// 'F'
#define EI_CLASS	4
#define EI_DATA		5
#define	EI_VERSION	6
#define EI_OSABI	7	// Targeted operating system
#define EI_ABIVERSION	8

/* ELF Header Flags */
#define EF_ARM_ABIMASK		0xFF000000
#define EF_ARM_BE8		0x00800000
#define EF_ARM_GCCMASK		0x00400FFF
#define EF_ARM_ABI_FLOAT_HARD	0x00000400
#define EF_ARM_ABI_FLOAT_SOFT	0x00000200

/* Defines what type of file it is */
#define no_file_type	0
#define relocatable	1
#define	executable	2
#define	shared_obj	3
#define	core		4

/* Types of Program Headers */
#define PT_NULL		0	// Indicates an unused program header
#define PT_LOAD		1	// Indicates that this program header describes a segment to be loaded from the file. 
#define PT_DYNAMIC	2	// Indicates a segment where dynamic linking information can be found. 
#define PT_INTERP	3	// Indicates a segment where the name of the program interpreter may be found.
#define PT_NOTE		4	// Indicates a segment holding note information. 
#define PT_SHLIB	5	// A reserved program header type, defined but not specified by the ELF ABI. 
#define PT_PHDR		6	// Indicates a segment where the program headers may be found. 

/* Program Flags */
// 0:			No access
// 1:			Just execute
// 2:			Write only
// 3:			Write, execute only
// 4:			Read only
// 5:			Read, execute only
// 6:			Read, write only
// 7:			Read, Write, and Execute

typedef enum {TEXT, RODATA, DATA, SYMTAB, STRTAB, SHSTRTAB, BSS, COMMENT} Section;

typedef struct {
	unsigned char 	e_ident[EI_NIDENT];	// How to interpret file
	Elf_Half 	e_type;
	Elf_Half 	e_machine;
	Elf_Word	e_version;
	Elf_Addr	e_entry;
	Elf_Off 	e_phoff;
	Elf_Off		e_shoff;
	Elf_Word	e_flags;
	Elf_Half	e_ehsize;
	Elf_Half	e_phentsize;
	Elf_Half	e_phnum;
	Elf_Half	e_shentsize;
	Elf_Half	e_shnum;
	Elf_Half	e_shstrndx;
}Elf_Ehdr;

/* Program header */
typedef struct {
	Elf_Word 	p_type;
	Elf_Off		p_offset;
	Elf_Addr	p_vaddr;
	Elf_Addr	p_paddr;
	Elf_Word	p_filesz;
	Elf_Word	p_memsz;
	Elf_Word	p_flags;
	Elf_Word	p_align;
}Elf_Phdr;

/* Section header */
typedef struct {
	Elf_Word	sh_name;
	Elf_Word	sh_type;
	Elf_Word	sh_flags;
	Elf_Addr	sh_addr;
	Elf_Off		sh_offset;
	Section 	sh_numname;
	Elf_Word	sh_size;
	Elf_Word	sh_link;
	Elf_Word	sh_info;
	Elf_Word	sh_addralign;
	Elf_Word	sh_entsize;
}Elf_Shdr;

/* Symbol Table */
typedef struct {
	Elf_Word	st_name;
	Elf_Addr	st_value;
	Elf_Word	st_size;
	unsigned char	st_info;
	unsigned char	st_other;
	Elf_Half	st_shndx;
}Elf_Sym;

/* Relocation without addend */
typedef struct {
	Elf_Addr 	r_offset;
	Elf_Word	r_info;
}Elf_Rel;

/* Relocation with addend */
typedef struct {
	Elf_Addr	r_offset;
	Elf_Word	r_info;
	Elf_Sword	r_addend;
}Elf_Rela;

/* Dynmaic Section header */
typedef struct {
	Elf_Sword	d_tag;
	union {
		Elf_Word	d_val;
		Elf_Addr	d_ptr;
	}d_un;
} Elf_Dyn;

int read_elf_header(Elf_Ehdr *h, unsigned char *pointer);
void read_program_header_table(Elf_Ehdr *eh, Elf_Phdr ph[], unsigned char *pointer);
void read_section_header_table(Elf_Ehdr *eh, Elf_Shdr sh[], uint32_t *pointer);
void parse_section_header_names(Elf_Ehdr *eh, Elf_Shdr sh[], uint32_t *pointer);
#endif
