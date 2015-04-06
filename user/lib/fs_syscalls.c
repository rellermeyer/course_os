#include <stdio.h>
#include <stdint.h>
#include "../../kernel/include/hw_handler.h"
#include "../../kernel/include/open_table.h"


/*open() returns a file descriptor...which is a small non-negative int value
  that corresponds to the index of the file in the system wide open file
  description table*/
opentable_entry* open(char* filepath, char mode){
	opentable_entry* fd;
	// preserve registers r1, r2, and r3 on the stack
    asm volatile("push r1" "\n\t"
                 "push r2" "\n\t"
                 "push r3" "\n\t");

	// move the arguments into registers for kopen() to use:
	asm volatile("mov %0, (filepath)": "r1");
	asm volatile("mov %0, (mode)": "r2");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_OPEN");
	// kopen() gets called from within hw_handler.c

	
	// retrieve the return values from kopen to pass back to user:
	asm volatile("mov %0, r1" : "=r" (fd));

	// Restore r1, r2, and r3 to original values
    asm volatile("pop r3" "\n\t"
                 "pop r2" "\n\t"
                 "pop r1" "\n\t");
	return fd;
}//end open syscall

/*read() returns a file descriptor...which is a small non-negative int value
  that corresponds to the index of the file in the system wide open file
  description table*/
int read(opentable_entry* fd, void* buf, int numBytes){
	if (numBytes<=0)
		os_printf(“Invalid number of bytes \n”);
	int bytesRead; //this will be what we return

    // preserve registers r1, r2, and r3 on the stack
    asm volatile("push r1" "\n\t"
                 "push r2" "\n\t"
                 "push r3" "\n\t");

	// move the arguments into registers for kread to use:
	asm volatile("mov %0, (fd)": "r1");
	asm volatile("mov %0, (buf)": "r2")
	asm volatile("mov %0, (numBytes)": "r3");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_READ");
	// kread() gets called from within hw_handler.c

	// retrieve the return values from kread to pass back to user:
	asm volatile("mov %0, r1" : "=r" (bytesRead));

	// Restore r1, r2, and r3 to original values
    asm volatile("pop r3" "\n\t"
                 "pop r2" "\n\t"
                 "pop r1" "\n\t");
	returns bytesRead;
	//note, actual data read from file gets moved to buf in kread()
}//end open syscall

/*write() returns the number of bytes that were actually written*/
int write(opentable_entry* fd, void* buf, int numBytes){
	if (numBytes<=0)
		os_printf(“Invalid number of bytes \n”);
	int bytesWritten; //this will be what we return

	// preserve registers r1, r2, and r3 on the stack
    	asm volatile("push r1" "\n\t"
                     "push r2" "\n\t"
                     "push r3" "\n\t");

	// move the arguments into registers for kwrite to use:
	asm volatile("mov %0, (fd)": "r1");
	asm volatile("mov %0, (buf)": "r2");
	asm volatile("mov %0, (numBytes)": "r3");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_WRITE");
	// kwrite() gets called from within hw_handler.c	

	// retrieve the return values from kwrite() to pass back to user:
	asm volatile("mov %0, r1" : "=r" (bytesWritten));

	// Restore r1, r2, and r3 to original values
    asm volatile("pop r3" "\n\t"
                 "pop r2" "\n\t"
                 "pop r1" "\n\t");
	return bytesWritten;
	//note, actual data written from file gets moved to buf in kwrite()
}//end write syscall


/*close() returns the 0 if successful and -1 if not*/
int close(opentable_entry* fd){
	int error; //0 if close ok, -1 if error. This will be return.

	// move the arguments into registers for kclose to use:
	asm volatile("mov %0, (fd)": "r1");//note, <register> not necessarily correct...need to figure out which register to use

	// preserve registers r1, r2, and r3 on the stack  —— need all 3?
    asm volatile("push r1" "\n\t"
                 "push r2" "\n\t"
                 "push r3" "\n\t");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_CLOSE");
	// kclose() gets called from within hw_handler.c	


	// retrieve the return values from kread to pass back to user:
	asm volatile("mov %0, r1" : "=r" ());

	// Restore r1, r2, and r3 to original values
    asm volatile("pop r3" "\n\t"
                 "pop r2" "\n\t"
                 "pop r1" "\n\t");
	return error;
}//end close syscall

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
