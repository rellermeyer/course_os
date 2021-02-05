#ifndef COURSE_OS_ELF_H
#define COURSE_OS_ELF_H

#include <stdint.h>
#include <vp_array_list.h>

// Taken from: http://www.skyfree.org/linux/references/ELF_Format.pdf
#define EI_NIDENT 16
#define Elf32_Addr uint32_t
#define Elf32_Half uint16_t
#define Elf32_Off uint32_t
#define Elf32_Sword uint32_t
#define Elf32_Word uint32_t

typedef struct ElfProgram {
    uint32_t file_offset;
    uint32_t file_size;
    uint32_t mem_location;
    uint32_t mem_size;
} ElfProgram;

typedef struct ElfSection {
    void *address;
    void *offset;
    uint32_t size;
    bool writable;
    bool allocate;
    bool instructions;
} ElfSection;

typedef struct Elf {
    void *entry;
    void *binary;
    VPArrayList *sections;
    ElfProgram program;
} Elf;

typedef struct ElfRawHeader {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} ElfRawHeader;

typedef struct ElfRawProgram {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_a;
} ElfRawProgram;

typedef struct ElfRawSection {
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} ElfRawSection;

Elf *elf_decode(void *binary, int length);

#endif //COURSE_OS_ELF_H
