#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vp_array_list.h>

Elf *elf_parse_header(Elf * elf, ElfRawHeader *elf_raw_header);
ElfSection *elf_parse_section(Elf * elf, ElfRawSection *elf_raw_section);
Elf *elf_parse_program(Elf *elf, ElfRawProgram *elf_raw_program);

/**
 * Parse a binary array as ELF file.
 * Returns NULL if invalid.
 */
Elf *elf_decode(void *binary, int length) {
    TRACE("[ELF] Starting parsing of an ELF file.");
    Elf *elf = kmalloc(sizeof(Elf));
    elf->binary = binary;

    // Parse the header
    ElfRawHeader *header = (void *) binary;
    if(elf_parse_header(elf, header) == NULL) {
        kfree(elf);
        return NULL;
    }

    // Parse sections
    for(int i = 0; i < header->e_shnum; i++) {
        ElfRawSection *rawSection = (void *) binary + header->e_shoff + (i * header->e_shentsize);
        ElfSection *section = elf_parse_section(elf, rawSection);

        if(section == NULL) {
            kfree(elf);
            return NULL;
        }

        vpa_push(elf->sections, section);
    }

    // Parse program
    ElfRawProgram *rawProgram = (void *) binary + header->e_phoff;
    if(elf_parse_program(elf, rawProgram) == NULL) {
        kfree(elf);
        return NULL;
    }

    return elf;
}

/**
 * Parse the header of the ELF file.
 * Returns null if the file was invalid.
 */
Elf *elf_parse_header(Elf *elf, ElfRawHeader *elf_raw_header) {
    // Check the magic bytes
    char magic_byte[] = {0x7f, 0x45, 0x4c, 0x46};
    if(memcmp(elf_raw_header->e_ident, magic_byte, 4) != 0) {
        WARN("[ELF] Invalid magic bytes.");
        return NULL;
    }

    // Check the file class
    if(elf_raw_header->e_ident[4] != 0x01) {
        WARN("[ELF] ELF file not made for 32-bit devices.");
        return NULL;
    }

    // Check the data encoding
    if(elf_raw_header->e_ident[5] != 0x01) {
        WARN("[ELF] ELF file not in the correct data encoding.");
        return NULL;
    }

    // Check the file version
    if(elf_raw_header->e_ident[6] != 0x01) {
        WARN("[ELF] ELF file does not have the correct version.");
        return NULL;
    }

    // Check the type of file
    if(elf_raw_header->e_type != 0x02) {
        WARN("[ELF] ELF file is not an executable.");
        return NULL;
    }

    // Check the target machine
    if(elf_raw_header->e_machine != 0x28) {
        WARN("[ELF] ELF file is not targeted for ARM.");
        return NULL;
    }

    // Check the version
    if(elf_raw_header->e_version != 0x01) {
        WARN("[ELF] ELF file does not have the correct version.");
        return NULL;
    }

    // Check the program headers
    if(elf_raw_header->e_phnum != 0x01) {
        WARN("[ELF] ELF files with more than one program header are currently unsupported.");
        return NULL;
    }

    elf->entry = (void *) elf_raw_header->e_entry;
    TRACE("[ELF] Executable entry defined on: 0x%x.", elf->entry);

    elf->sections = vpa_create(elf_raw_header->e_shnum);
    TRACE("[ELF] Amount of sections: %i.", elf_raw_header->e_shnum);

    return elf;
}

ElfSection *elf_parse_section(Elf *elf, ElfRawSection *elf_raw_section) {
    ElfSection *section = kmalloc(sizeof(ElfSection));

    section->address = (void *) elf_raw_section->sh_addr;
    TRACE("[ELF] Section address start: 0x%x.", section->address);

    section->offset = (void *) elf_raw_section->sh_offset;
    TRACE("[ELF] Section file offset: 0x%x.", section->offset);

    section->size = elf_raw_section->sh_size;
    TRACE("[ELF] Section size: %i bytes.", section->size);

    section->writable = (bool) elf_raw_section->sh_flags & 0x1;
    TRACE("[ELF] Section writable: %i.", section->writable);

    section->allocate = (bool) ((elf_raw_section->sh_flags & 0x2) >> 1);
    TRACE("[ELF] Section needs allocation: %i.", section->allocate);

    section->instructions = (bool) ((elf_raw_section->sh_flags & 0x4) >> 2);
    TRACE("[ELF] Section contains instructions: %i.", section->instructions);

    TRACE("[ELF] *** End parsing Section ***");
    return section;
}

Elf *elf_parse_program(Elf *elf, ElfRawProgram *elf_raw_program) {
    if(elf_raw_program->p_type != 0x01) {
        WARN("[ELF] Non-loadable segments are currently unsupported.");
        return NULL;
    }

    elf->program.file_offset = elf_raw_program->p_offset;
    TRACE("[ELF] Program file offset: 0x%x.", elf->program.file_offset);

    elf->program.mem_location = elf_raw_program->p_vaddr;
    TRACE("[ELF] Program memory location: 0x%x.", elf->program.mem_location);

    elf->program.file_size = elf_raw_program->p_filesz;
    TRACE("[ELF] Program file size: 0x%x.", elf->program.file_size);

    elf->program.mem_size = elf_raw_program->p_memsz;
    TRACE("[ELF] Program memory size: 0x%x.", elf->program.mem_size);

    if(elf->program.file_size != elf->program.mem_size) {
        WARN("[ELF] File size is unequal to the memory size. We don't yet know how to handle that.");
        return NULL;
    }

    return elf;
}

/**
 * Free a ELF section struct.
 */
void elf_section_free(ElfSection *section) {
    if(section == NULL) return;

    kfree(section);
}

/**
 * Free a ELF struct.
 */
void elf_free(Elf *elf) {
    vpa_free(elf->sections, (FreeFunc) elf_section_free);
    kfree(elf);
}
