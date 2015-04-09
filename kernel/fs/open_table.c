//This file contains the table of open files, implemented as an array
// with methods like add_to_opentable and delete_from_opentable to be called in open and close 
//Since methods are provided, other files should not touch neither the free LL or the array, 
// but just use the premade methods. 

#include "../include/file.h" 
#include "../include/klibc.h"

//number of files that can fit in the table
#define SYSTEM_SIZE 512; 


// Each cell of the array is a struct with infos about the file.
// More fields can be added if necessary.
typedef struct file_descriptor 
{
 	struct file * linked_file; //pointer to file struct 
 	int offset; //keeps track of where we are reading
} file_descriptor;

// Array of open files 
struct file_descriptor table[SYSTEM_SIZE];


// LL of free indexes at which we can place a newly opened file.
typedef struct free_index {
	int index;
	struct free_index * next;
} free_index;

// FIFO structure. When a file is closed, it is added to the tail. 
// When requested to open a file, the index is taken from the head. 
free_index * HEAD;
free_index * TAIL;

//called by start.c, will initialize the free list and the table
void fs_table_init()
{
	free_index * behind = kmalloc(sizeof(struct free_index));
    HEAD = behind;
    HEAD->index = 0;

    int i;
    int max = SYSTEM_SIZE;
    for (i=1; i<max; i++) {
        free_index * ahead = kmalloc(sizeof(struct free_index));
        ahead->index = i;
        behind->next = ahead;
        behind = ahead;
    } 
    TAIL = behind;
	//note: this memory should be freed with a cleanup function once the computer turns off
 	//since there is no turn off at the moment, the function is still fo be implemented
}

// this function can be used to insert a file in the table
// and returns the requested index if successful, else -1 
int add_to_opentable(struct file * f) 
{
	if (HEAD==NULL) //not enough space
		return -1; 	
	int fd = HEAD->index; //take available fd
	free_index * free_me = HEAD; 
    HEAD = HEAD->next; //dequeue
	kfree(free_me); //free old node
	file_descriptor to_add; //initialize the struct
	to_add.linked_file = f;
	to_add.offset = 0; 
	table[fd] = to_add; //add to table ---> hopefully this copies the struct and not only a pointer!
	return fd;
}


//this function can be used to delete a file from the list
//returns 0 if all ok, -1 if wrong
int delete_from_opentable(int fd)
{
	if (table[fd]==NULL) 
		return -1; //invalid entry
	table[fd]=NULL;
	free_index * to_add = kmalloc(sizeof(struct free_index)); //create new node
	to_add->index = fd;
	TAIL->next = to_add;
	TAIL = to_add; //enqueue

}

//this function checks whether the file is open or not
int file_is_open(int fd) 
{
	return (table[fd]!=NULL);
}
