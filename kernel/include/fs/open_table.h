#ifndef __OPEN_TABLE_H__
#define __OPEN_TABLE_H__

//header for the open table file
//that implements the open table for the file system
//please use provided functinos to add/remove items 

#include "global_defs.h"
#include "klibc.h"
#include "fs/file.h"
#include "data_structures/bitvector.h"

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


void fs_table_init(); //initializes fs open table at boot up time
void fs_table_shutdown(); //closes the open table at shutdown time

int add_to_opentable(struct inode* f, char perm);  //adds a file to the opentable, returns fd if successful and -1 if not

int delete_from_opentable(int fd); //deletes a file from the opentable, returns 0 if successful, -1 if not

int file_is_open(int fd); //checks if there is an entry corrensponding to that fd

int inode_is_open(struct inode* cur_inode);

struct file_descriptor* get_descriptor(int fd); //returns the filedescriptor struct linked to fd 

#endif 
