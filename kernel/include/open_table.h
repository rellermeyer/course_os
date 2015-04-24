#ifndef __OPEN_TABLE_H__
#define __OPEN_TABLE_H__

//header for the open table file
//that implements the open table for the file system
//please use provided functinos to add/remove items 

#define SYSTEM_SIZE 512 

// Each cell of the array is a struct with infos about the file.
// More fields can be added if necessary.
struct file_descriptor {
	char permission;
    struct inode* linked_file; //pointer to file struct 
    int offset; //keeps track of where we are reading
};

// Array of open files 
struct file_descriptor* table[SYSTEM_SIZE]; 

// LL of free indexes at which we can place a newly opened file.
struct free_index {
    int index;
    struct free_index* next;
};

void fs_table_init(); //initializes fs open table at boot up time

int add_to_opentable(struct inode*, char);  //adds a file to the opentable
					//returns fd if successul
					//and -1 if unsuccessful

int delete_from_opentable(int);      //deletes a file from the opentable
					//returns 0 if successful
					//and -1 if unsuccessful

int file_is_open(int); //checks if there is an entry corrensponding to that fd

struct file_descriptor* get_descriptor(int);
//cleanup functinos to be implemented when shutdown will be implemented

#include "file.h"

#endif 
