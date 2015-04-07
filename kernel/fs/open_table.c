//this file contains the table of open files, implemented as a double linked list
//with methods like add_to_opentable and delete_from_opentable to be called in open and close 

#include "file.h"  //add directory if necessary

//only has fd and pointers. More fields can be added if necessary.
// typedef struct file_descriptor 
// {
// 	file* file;
// 	struct file_descriptor * previous;
// 	// int fd; //file descriptor
// 	int offset; //keeps track of where we are reading
// 	struct file_descriptor * next;	
// 	int open;
// } file_descriptor;

// beginning of list 
file_descriptor * HEAD;

// end of list
file_descriptor * TAIL;


//this function can be used to insert a file in the list and returns a pointer to it
file_descriptor* add_to_opentable(file_descriptor* fd_to_add) 
{
	file_descriptor to_add;
	to_add->fd = fd_to_add;

	if (HEAD==NULL) //empty list
	{
		HEAD=to_add;
		TAIL=to_add;
	}

	else //add to existing list
	{
		TAIL->next=to_add;
		to_add->previous=TAIL;
		TAIL=to_add;
	}
	return to_add;
}

//this function can be used to delete a file from the list
//returns 0 if all ok, -1 if wrong
int delete_from_opentable(file_descriptor* fd_to_delete)
{
	if (HEAD==NULL)
		return -1; //error
	
	if (fd_to_delete==HEAD ^ fd_to_delete==TAIL) { 
		HEAD=NULL;
		TAIL=NULL;
	}
	else if (fd_to_delete==HEAD)
		HEAD=fd_to_delete->next;
	else if (fd_to_delete==TAIL)
		TAIL=fd_to_delete->previous;
	else {
		fd_to_delete->previous->next=fd_to_delete->next;
		fd_to_delete->next->previous=fd_to_delete->previous;
	}
	return 0;
}
