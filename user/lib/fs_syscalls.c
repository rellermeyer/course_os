#include <stdio.h>
#include <stdint.h>
#include "../../kernel/include/hw_handler.h"

/*open() returns a file descriptor...which is a small non-negative int value
  that corresponds to the index of the file in the system wide open file
  description table*/
int open(char* filepath, char mode){
	int fd;
	//assembly code to move the arguments into registers for kopen() to use:
	asm volatile("mov %0, (filepath)": "<register>");//note, <register> not necessarily correct...need to figure out which register to use
	asm volatile("mov %0, (mode)": "<register>");//note, <register> not necessarily correct...need to figure out which register to use

	//assembly code to trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_OPEN");
	
	//assembly code to retrieve the return values from kopen to pass back to user:
	asm volatile("mov %0, <register>" : "=r" (fd));
	return fd;
}//end open syscall

/*read() returns a file descriptor...which is a small non-negative int value
  that corresponds to the index of the file in the system wide open file
  description table*/
int read(int fd, void* buf, int numBytes){
	//note, not sure if we need to allocated memory for buf, or if the user is responsible for that...
	int bytesRead; //this will be what we return:()
	//assembly code to move the arguments into registers for kread to use:
	asm volatile("mov %0, (fd)": "<register>");//note, <register> not necessarily correct...need to figure out which register to use
	asm volatile("mov %0, (buf)": "<register>");//note, <register> not necessarily correct...need to figure out which register to use
	asm volatile("mov %0, (numBytes)": "<register>");//note, <register> not necessarily correct...need to figure out which register to use

	//assembly code to trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_READ");

	//assembly code to retrieve the return values from kread to pass back to user:
	asm volatile("mov %0, <register>" : "=r" (bytesRead));
	returns bytesRead;
	//note, actual data read from file gets moved to buf in kopen()
}//end open syscall


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
