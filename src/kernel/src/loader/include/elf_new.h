#include <stdint.h>
 
typedef uint16_t Elf32_Half;	// Unsigned half int
typedef uint32_t Elf32_Off;	    // Unsigned offset
typedef uint32_t Elf32_Addr;	// Unsigned address
typedef uint32_t Elf32_Word;	// Unsigned int
typedef int32_t  Elf32_Sword;	// Signed int

//----------------------------------------
// The ELF Header
# define ELF_MN_ELF 4
# define PADDING_LENGTH 8
 
typedef struct {
    // Identification Information
	uint8_t		magic_sequence[ELF_MN_ELF];
    uint8_t     bit_32_or_64;
    uint8_t     type_of_endian;
    uint8_t     header_version;
    uint8_t     OS_ABI;::
    uint8_t     padding[PADDING_LENGTH];

	Elf32_Half	e_type;
	Elf32_Half	e_architecture;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_program_header_table_offset;
	Elf32_Off	e_section_header_table_offset;
	Elf32_Word	e_flags;
	Elf32_Half	e_header_size;
	Elf32_Half	e_program_header_table_entry_size;
	Elf32_Half	e_program_header_table_num_entries;
	Elf32_Half	e_section_header_table_entry_size;
	Elf32_Half	e_section_header_table_num_entries;
	Elf32_Half	e_index_sec_head_tb_entry_names;
} Elf32_Header;

// The magic number: 7FELF
enum Elf_Magic {
	EI_MAG0		= 0, // 0x7F
	EI_MAG1		= 1, // 'E'
	EI_MAG2		= 2, // 'L'
	EI_MAG3		= 3, // 'F'
};
 
# define ELFMAG0	0x7F // e_ident[EI_MAG0]
# define ELFMAG1	'E'  // e_ident[EI_MAG1]
# define ELFMAG2	'L'  // e_ident[EI_MAG2]
# define ELFMAG3	'F'  // e_ident[EI_MAG3]
 
# define ELFDATA2LSB	(1)  // Little Endian
# define ELFCLASS32	    (1)  // 32-bit Architecture

enum Elf_Type {
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};
 
# define ELF_ARCH_ARM32	(0x28)  // ARM Machine Type ( 32-bit ARM is just ARM)
# define EV_CURRENT	(1)         // ELF Current Version
// -----------------------------------------------------


// TODO - Structures for a program header table
// TODO - Structure  for a section header table
// TODO - Structure  for a program header table entry
// TODO - Structure  for a section header table entry
// TODO - Structure  for the elf file

// TODO - method which takes the path to a file and uses the file system to load
// 		  it into memory for reading

// TODO - method that takes a pointer to an ELF file in memory and begins its processing  

// Cheching the ELF Header
// Before we process in any way an ELF file, we have to check if the machine is able to do it
// We have to check:
// 1) Validity  of the ELF file
// 2) Architecture correspondence
// 3) Endianess
// 4) CPU type
// 5) Other OS specific semantics are satisfied

/* A function to validate that the magic number in the 
   ELF file header is correct.
   @return true if correct, false otherwise
*/
bool elf_validate_magic_sequence(Elf32_Header* header);

