/* Any confusion with what does what can be gotten from the 
   following website. It defines all the values
	http://man7.org/linux/man-pages/man5/elf.5.html
*/

#ifndef _ELF_H_
#define _ELF_H_

typedef u32 Elf_Addr;	// Program Address
typedef u16 Elf_Half;	// 16 bit 
typedef u32 Elf_Off;	// File Offset
typedef s32 Elf_Sword;	// Signed 32 bit int
typedef u32 Elf_Word;	// Unsigned 32 bit int
#define EI_NIDENT	16

/* Elf Header */
/* Note: The ELF Header should begin the following header
	EI_MAG0 = 0x7f
	EI_MAG1 = 'E'
	EI_MAG2 = 'L'
	EI_MAG3 = 'F'
*/
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
extern Elf_Dyn_DYNAMIC[];
