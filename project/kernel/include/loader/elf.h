#include <stdint.h>
#include <stdbool.h>

 
typedef uint16_t Elf32_Half;	// Unsigned half int
typedef uint32_t Elf32_Off;	    // Unsigned offset
typedef uint32_t Elf32_Addr;	// Unsigned address
typedef uint32_t Elf32_Word;	// Unsigned int

//----------------------------------------
// The ELF Header
# define ELF_MN_ELF 4
# define PADDING_LENGTH 8

# define ELFMAG0	0x7F // e_ident[EI_MAG0]
# define ELFMAG1	'E'  // e_ident[EI_MAG1]
# define ELFMAG2	'L'  // e_ident[EI_MAG2]
# define ELFMAG3	'F'  // e_ident[EI_MAG3]

enum Elf_Magic {
    EI_MAG0		= 0, // 0x7F
    EI_MAG1		= 1, // 'E'
    EI_MAG2		= 2, // 'L'
    EI_MAG3		= 3, // 'F'
};

enum Elf_Type {
    ET_NONE,    // Unknown type
    ET_REL,     // Relocatable object file
    ET_EXEC,    // Static executable file
    ET_DYN      // Dynamic executables and shared object files
};
 
typedef struct {
    // Identification Information
    uint8_t	magic_sequence[ELF_MN_ELF];
    uint8_t     bit_32_or_64;
    uint8_t     type_of_endian;
    uint8_t     header_version;
    uint8_t     OS_ABI;
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

// end Header ---------------------------------------------------


//------------------------------------------------
// The Program Header

#define PROGRAM_TYPES 7
#define SUPPORTED_PROGRAM_TYPES 4

// Program Types
enum ELF_PROGRAM_TYPE {
    EPT_NULL,
    EPT_LOAD,
    EPT_DYNAMIC,
    EPT_INTERP,
    EPT_NOTE,
    PT_SHLIB,
    PT_PHDR,
};

extern enum ELF_PROGRAM_TYPE validEPTypes[SUPPORTED_PROGRAM_TYPES];
extern char * program_types[PROGRAM_TYPES];

#define strloaderr(num) program_types[num]

// Program Flags
enum ELF_PROGRAM_FLAG {
    EPF_EXEC = 1,
    EPF_WRITE = 2,
    EPF_READ = 4
};

typedef struct {
	Elf32_Word	program_type;
	Elf32_Off	program_offset;
	Elf32_Addr	program_virtual_addr;
	Elf32_Addr	program_physical_addr;
	Elf32_Word	program_filesz;
	Elf32_Word	program_memsz;
	Elf32_Word	flags;
	Elf32_Word	align;

} Elf32_ProgramHeader;

// End Program Header -------------------------------------------


// -----------------------------------------
// The Section Header

typedef struct {
    Elf32_Word section_name;
    Elf32_Word section_type;
    Elf32_Word section_flags;
    Elf32_Addr section_addr;
    Elf32_Off  section_offset;
    Elf32_Word section_size;
    Elf32_Word section_link;
    Elf32_Word section_info;
    Elf32_Word section_addr_align;
    Elf32_Word section_entry_size;
} Elf32_SectionHeader;

// End Section Header -------------------------------------------


// -----------------------------------------
// A structure for an ELF file, containing the
// information from the parsed ELF header.
typedef struct Elf {
    Elf32_Addr entry;
    Elf32_SectionHeader * section_names_address;
    Elf32_SectionHeader * sectionHeaderTableAddress;
    Elf32_ProgramHeader * programHeaderTableAddress;
    Elf32_Word sectionHeaderTableLength;
    Elf32_Word programHeaderTableLength;
} Elf;
// End -----------------------------------------------


// -----------------------------------------
// Function declarations

// Checking the ELF Header
// Before we process in any way an ELF file, we have to check if the machine is able to do it
// We have to check:
// 1) Validity of the ELF file - the magic sequence
// 2) Architecture correspondence
// 3) Endianess
// 4) CPU type
// 5) Other OS specific semantics are satisfied
// This is done in the following two methods: elf_validate_magic_sequence and elf_validate_header_support

/*
 * A function to validate that the magic number in the
 * ELF file header.
 *
 * @return 0 if correct, negative error code otherwise
 * */
int elf_validate_magic_sequence(Elf32_Header* header);


/*
 * A function to validate that the ELF file being
 * processed has the correct characteristics in
 * its header information.
 *
 * @return 0 if correct, negative error code otherwise
 */
int elf_validate_header_support(Elf32_Header* header);

/*
 * A function to parse the information contained in the ELF file
 * header, including the program and section header tables' sizes
 * and locations and the entry point of the process to create.
 *
 * @return 0 if correct, negative error code otherwise
 */
int elf_parse_header(Elf * elf, Elf32_Header *elf_raw_header);

/*
 * A function to check if a program header is a valid one.
 * It checks if the filesize is no bigger than memsize.
 * It also checks if the type of the program header is a
 * supported one.
 *
 * @return 0 if correct, negative error code otherwise
 */
int elf_validate_program_header(Elf32_ProgramHeader * header);

/*
 * A function that iterates over the list of supported
 * segment types and checks if a provided type is part
 * of it.
 *
 * @return 0 if correct, negative error code otherwise
 */
int elf_validate_program_type_support(enum ELF_PROGRAM_TYPE type);