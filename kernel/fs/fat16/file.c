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

/* read from fd, put it in buf, then return the number of bytes read in numBytes */
int kread(int fd, void* buf, int numBytes) {
	int bytesRead;

	return bytesRead;
} // end kread();

/* write from fd, put it in buf, then return the number of bytes written in numBytes */
int kwrite(int fd, void* buf, int numBytes) {
	int bytesWritten;

	return bytesWritten;
} // end kwrite();

/* close the file fd, return 1 if the close was successful */
int kclose(int fd) {
	int error;

	return error;
} // end kclose();

/* seek within the file, return an error if you are outside the boundaries */
int kseek(int fd, int numBytes) {
	int error;

	return error;
} // end kseek();

/* delete the file with the path filepath. Return -1 if the file does not excist */
int kdelete(char* filepath) {
	int error;

	return error;
} // end kdelete();









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



