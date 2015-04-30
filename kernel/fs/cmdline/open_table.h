#ifndef OPEN_TABLE_H
#define OPEN_TABLE_H

#include "bitvector.h"

#define SYSTEM_SIZE 512 //how many files can be open at the same time

// Each cell of the array is a struct with infos about the file.
// More fields can be added if necessary.
struct file_descriptor {
        char permission;
    struct inode* linked_file; //pointer to inode struct 
    int offset; //keeps track of where we are reading
};

// Array of open files 
struct file_descriptor** table; 

//list of free indexes
bit_vector * open_table_free_list;

#endif
