//This file contains the table of open files, implemented as an array
// with methods like add_to_opentable and delete_from_opentable to be called in open and close 
//Since methods are provided, other files should not touch neither the free LL or the arrray, 
// but just use the premade methods. 

#include "../include/file.h"
#include "../include/klibc.h"
#include "../include/open_table.h"

//number of files that can fit in the table
// #define SYSTEM_SIZE 512; 

// FIFO structure. When a file is closed, it is added to the tail. 
// When requested to open a file, the index is taken from the head. 
struct free_index* HEAD;
struct free_index* TAIL;

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
}

//at shutdown, memory with the free list is freed
void fs_table_shutdown() {
        struct free_index * to_free;
        while (HEAD != 0x0) {
                to_free = HEAD;
                HEAD = HEAD->next;
                kfree(to_free);
        }
}


//returns struct of descriptor at index fd
//if invalid, returns NULL.
struct file_descriptor* get_descriptor(int fd){
	if (file_is_open(fd))
		return table[fd];
	return 0x0;
}

// this function can be used to insert a file in the table
// and returns the requested index if successful, else -1 
int add_to_opentable(struct inode * f, char perm) {
        if (HEAD==0x0) //not enough space
                return -1;
        int fd = HEAD->index; //take available fd
	if (fd<0 || fd>=SYSTEM_SIZE) //invalid index
		return -1;
        struct free_index * free_me = HEAD;
        HEAD = HEAD->next; //dequeue
        kfree(free_me); //free old node
        struct file_descriptor* to_add = (struct file_descriptor*) kmalloc(sizeof(struct file_descriptor));
        //initialize the struct
        int i;
        int inum = f->inum;
        for (i=0; i<SYSTEM_SIZE; i++) {
                if ((table[i]->linked_file)->inum == inum) {
                        (to_add->linked_file)->fd_refs++;
                        to_add->linked_file =  table[i]->linked_file  
                        to_add->permission = perm;
                        to_add->offset = 0;
                        table[fd] = to_add; //add to table
                        return fd;
                }
        }
        to_add->linked_file = f;
        to_add->linked_file->fd_refs = 1;
	to_add->permission = perm;
        to_add->offset = 0;
        table[fd] = to_add; //add to table
        if (perm == 'a') {
                table[fd]->offset = table[fd]->linked_file->size;
        }
        return fd;
}


//this function can be used to delete a file from the list
//returns 0 if all ok, -1 if wrong
int delete_from_opentable(int fd) {
        if (table[fd]==0x0)
                return -1; //invalid entry
        if (table[fd]->linked_file->fd_refs == 1) {
                table[fd]->linked_file->fd_refs--;
                kfree(table[fd]->linked_file);
        }
        else {
                table[fd]->linked_file->fd_refs--;
        }
        kfree(table[fd]);
        struct free_index * to_add = kmalloc(sizeof(struct free_index)); //create new node
        to_add->index = fd;
        TAIL->next = to_add;
        TAIL = to_add; //enqueue
        return 0;
}


//this function checks whether the file is open or not
int file_is_open(int fd) {
	if (fd<0 || fd>=SYSTEM_SIZE) { return 0; }
        return (table[fd]!=0x0);
}

