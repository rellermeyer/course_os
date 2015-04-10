//This file contains the table of open files, implemented as an array
// with methods like add_to_opentable and delete_from_opentable to be called in open and close 
//Since methods are provided, other files should not touch neither the free LL or the arrray, 
// but just use the premade methods. 

#include "../include/file.h"
#include "../include/klibc.h"

//number of files that can fit in the table
#define SYSTEM_SIZE 512; 


// Each cell of the array is a struct with infos about the file.
// More fields can be added if necessary.
struct file_descriptor {
        struct file * linked_file; //pointer to file struct 
        int offset; //keeps track of where we are reading
};

// Array of open files 
struct file_descriptor table[SYSTEM_SIZE];


// LL of free indexes at which we can place a newly opened file.
struct free_index {
        int index;
        struct free_index * next;
};

// FIFO structure. When a file is closed, it is added to the tail. 
// When requested to open a file, the index is taken from the head. 
struct free_index * HEAD;
struct free_index * TAIL;

//called by start.c, will initialize the free list and the table
void fs_table_init() {
        struct free_index * behind = kmalloc(sizeof(struct free_index));
        HEAD = behind;
        int i;
        int max = SYSTEM_SIZE-1;
        for (i=0; i<max; i++) {
                behind->index = i;
                struct free_index * ahead = kmalloc(sizeof(struct free_index));
                behind->next = ahead;
                behind = ahead;
        }
        behind->index = SYSTEM_SIZE-1;
        TAIL = behind;
        //note: this memory should be freed with a cleanup function once the computer turns off
        //since there is no turn off at the moment, the function is still fo be implemented
}


// this function can be used to insert a file in the table
// and returns the requested index if successful, else -1 
int add_to_opentable(struct file * f) {
        if (HEAD==NULL) //not enough space
                return -1;
        int fd = HEAD->index; //take available fd
	if (fd<0 || fd>=SYSTEM_SIZE) //invalid index
		return -1;
        struct free_index * free_me = HEAD;
        HEAD = HEAD->next; //dequeue
        kfree(free_me); //free old node
        struct file_descriptor to_add; //initialize the struct
        to_add.linked_file = f;
        to_add.offset = 0;
        table[fd] = to_add; //add to table
        return fd;
}


//this function can be used to delete a file from the list
//returns 0 if all ok, -1 if wrong
int delete_from_opentable(int fd) {
        if (table[fd]==NULL)
                return -1; //invalid entry
        table[fd]=NULL;
        struct free_index * to_add = kmalloc(sizeof(struct free_index)); //create new node
        to_add->index = fd;
        TAIL->next = to_add;
        TAIL = to_add; //enqueue
}


//this function checks whether the file is open or not
int file_is_open(int fd) {
        return (table[fd]!=NULL);
}

