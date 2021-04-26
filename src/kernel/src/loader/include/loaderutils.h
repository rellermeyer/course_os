//
// Created by mitko on 03/04/2021.
//

#ifndef COURSE_OS_LOADERUTILS_H
#define COURSE_OS_LOADERUTILS_H

/**
 *  This header file declares some useful debugging functions that can be used with ELF files
 * to display information from them.
 */


#include <elf.h>

/**
 * Function that prints the names of all sections in an ELF file.
 */
int printSectionNames(void * file, Elf32_SectionHeader * shtable, Elf32_Word t_size, Elf32_SectionHeader * section_names);


#endif //COURSE_OS_LOADERUTILS_H
