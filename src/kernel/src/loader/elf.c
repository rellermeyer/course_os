

#include <elf_common_features.h>
#include <elf.h>
#include <klibc.h>


enum ELF_PROGRAM_TYPE validEPTypes[SUPPORTED_PROGRAM_TYPES] = {
    EPT_NULL,
    EPT_LOAD,
    EPT_DYNAMIC,
    EPT_NOTE
};

char * program_types[] = {
        "TYPE NULL",
        "TYPE_LOAD",
        "TYPE_DYNAMIC",
        "TYPE_INTERP",
        "TYPE_NOTE",
        "TYPE_SHLIB",
        "TYPE_PHDB"
};

int elf_parse_header(Elf * elf, Elf32_Header *elf_header) {

    if(elf == NULL || elf_header == NULL) return -1;

    INFO("Header address: %p", elf_header);

    // Check the validity of the Header
    int result;

    if((result = elf_validate_magic_sequence(elf_header)) < 0) {
        kprintf("Invalid ELF File.\n");
        return result;
    }

    if ((result = elf_validate_header_support(elf_header)) < 0) {
        kprintf("Parsing of ELF file header failed!\n");
        return result;
    }

    elf->entry = elf_header->e_entry;
    elf->programHeaderTableAddress = (Elf32_ProgramHeader *)((char *)elf_header + elf_header->e_program_header_table_offset);
    elf->sectionHeaderTableAddress = (Elf32_SectionHeader *)((char *)elf_header + elf_header->e_section_header_table_offset);
    elf->programHeaderTableLength = elf_header->e_program_header_table_num_entries;
    elf->sectionHeaderTableLength = elf_header->e_section_header_table_num_entries;
    elf->section_names_address = elf->sectionHeaderTableAddress + elf_header->e_index_sec_head_tb_entry_names;

    return 0;
}

int elf_validate_magic_sequence(Elf32_Header* header) {
    if(header == NULL) return -1;
    if(header->magic_sequence[EI_MAG0] != ELFMAG0) {
        kprintf("ELF Header EI_MAG0 incorrect. \n");
        return -1;
    }
    if(header->magic_sequence[EI_MAG1] != ELFMAG1) {
        kprintf("ELF Header EI_MAG1 incorrect.\n");
        return -1;
    }
    if(header->magic_sequence[EI_MAG2] != ELFMAG2) {
        kprintf("ELF Header EI_MAG2 incorrect.\n");
        return -1;
    }
    if(header->magic_sequence[EI_MAG3] != ELFMAG3) {
        kprintf("ELF Header EI_MAG3 incorrect.\n");
        return -1;
    }
    return 0;
}

int elf_validate_header_support(Elf32_Header* header) {

    if(header->bit_32_or_64 != EXPECTED_ELF_TYPE) {
        kprintf("Unsupported ELF File Class.\n");
        return -1;
    }
    if(header->type_of_endian != EXPECTED_ENDIANESS) {
        kprintf("Unsupported ELF File byte order.\n");
        return -1;
    }
    if(header->e_architecture != EXPECTED_MACHINE_ISA) {
        kprintf("Unsupported ELF File target.\n");
        return -1;
    }
    if(header->e_version != EXPECTED_VERSION) {
        kprintf("Unsupported ELF File version.\n");
        return -1;
    }
    if(header->e_type != ET_REL && header->e_type != ET_EXEC) {
        kprintf("Unsupported ELF File type.\n");
        return -1;
    }
    if (header->e_header_size != EXPECTED_HEADER_SIZE) {
        kprintf("Invalid header size.\n");
        return -1;
    }
    return 0;

}

int elf_validate_program_header(Elf32_ProgramHeader * header) {
    if (!elf_validate_program_type_support(header->program_type)) {
        kprintf("ELF Program Invalid: Type is not supported: %s", strloaderr(header->program_type));
        return -1;
    }
    if (header->program_filesz > header->program_memsz) {
        kprintf("ELF Program Invalid: File size is bigger than Memory Size");
        return -1;
    }

    // TODO Validate other fields in case the program has LOAD type.
    return 0;
}

int elf_validate_program_type_support(enum ELF_PROGRAM_TYPE type) {
    for (int i = 0; i < SUPPORTED_PROGRAM_TYPES; ++i) {
        if (type == validEPTypes[i]) return true;
    }
    return false;
}
