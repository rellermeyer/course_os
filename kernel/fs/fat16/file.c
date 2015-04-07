#include <stdint>
#include "../../include/linked_list.h"
//NOTES:
//free list = keeps track of which blocks on the storage device are free (i.e. available)...must be stored persistently


//CONSTANTS:
const uint16_t BLOCKSIZE = 2048;


//STRUCTS:
typedef struct file
{
	/* data */
	char* fname;
	list inode;
	uint16_t size;
	bitvector perms; // a bitvector of length three to track: read, write, execute
	time creation_time; // need to look up CourseOS specific data type
	uint16_t offset;
} file;

// typedef struct inode
// {
// 	 data 

// } inode;

file_descriptor * kopen(char* filepath, char mode){
	file_descriptor* fd;
	// TODO implement


	return fd;
}//end kopen()

// int read(opentable_entry* fd, int numBytes)











/*put these in kopen() implementation:
	switch(mode){}
		case "r":
   	    //inline assembly code to move the filepath and mode arguments to registers to be used in kopen():
		
	       break;
	    case "w":
   	    //inline assembly code to move the filepath and mode arguments to registers to be used in kopen():

	       
	       break;
	    case "a":
   	    //inline assembly code to move the filepath and mode arguments to registers to be used in kopen():

	  	    break;
	    default :
			os_printf("Error! Must pass a valid mode parameter\n");
		}
*/



