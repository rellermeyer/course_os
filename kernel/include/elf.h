#ifndef _ELF_H_
#define _ELF_H_

typedef u32 Elf_Addr;	// Program Address
typedef u16 Elf_Half;	// 16 bit 
typedef u32 Elf_Off;	// File Offset
typedef s32 Elf_Sword;	// Signed 32 bit int
typedef u32 Elf_Word;	// Unsigned 32 bit int
#define EI_NIDENT	16
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
	Elf_Half	e_phunum;
	Elf_Half	e_shentsize;
	Elf_Half	e_shnum;
	Elf_Half	e_shstrndx;
}
