//
// Created by mitko on 03/04/2021.
//

#include <loaderutils.h>
#include <klibc.h>

int printSectionNames(void * file, Elf32_SectionHeader * shtable, Elf32_Word t_size, Elf32_SectionHeader * names_section) {

    // Get a pointer to the names section data
    Elf32_Addr names = (uint32_t)((char *)file + names_section->section_offset);

    while(--t_size >= 0) {

        // Do not make boundary checks for the offset into the section name table
        kprintf("Section name: %s \n", (char *)names + (shtable[t_size].section_name));

    }

    return 0;
}
