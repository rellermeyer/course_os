// #include <stdio.h>
#include <stdint.h>
#include "../../kernel/include/hw_handlers.h"
#include "../../kernel/include/open_table.h"
#include "../../kernel/include/klibc.h"

/*open() returns a file descriptor, an int used to call later fuctions.
A return value of -1 tells that an error occurred and the file cannot be opened*/
int open(char* filepath, char mode){
	if(mode != 'r' || mode != 'w' || mode != 'a' || mode!='b'){
		os_printf("mode is not a valid option. \n");
		os_printf("r=read, w=write, b=both read and write, a=append.\n");
		return -1;
	}//end if

	int fd =0;

	// move the arguments into registers for kopen() to use:
	  asm volatile("mov %[filepath1], r1"::[filepath1]"r" (filepath): "r1");
	  asm volatile("mov %[mode1], r2"::[mode1]"r" (mode): "r2");

	// trigger the software_interrupt_handler in hw_handler.c:
	  asm volatile("swi SYSCALL_OPEN");
	// kopen() gets called from within hw_handler.c

	// retrieve the return values from kopen to pass back to user:
	  asm volatile("mov r1, %[fd1]":[fd1]"=r" (fd)::);

	return fd;
}//end open syscall
 
/*create() returns a file descriptor to use in later functions*/
int create(char* filepath, char mode){

	if(mode != 'r' || mode != 'w' || mode != 'a' || mode!='b'){
		os_printf("mode is not a valid option. \n");
		os_printf("r=read, w=write, b=both read and write, a=append.\n");
		return -1;
	}//end if

	int error =0;

	// move the arguments into registers for kcreate() to use:
	  asm volatile("mov %[filepath1], r1"::[filepath1]"r" (filepath): "r1");
	  asm volatile("mov %[mode1], r2"::[mode1]"r" (mode): "r2");

	// trigger the software_interrupt_handler in hw_handler.c:
	  asm volatile("swi SYSCALL_CREATE");
	// kopen() gets called from within hw_handler.c

	// retrieve the return values from kopen to pass back to user:
	  asm volatile("mov r1, %[error1]":[error1]"=r" (error)::);

	return error;
}//end create syscall

/*read() returns the amount of bytes successfully read from the file*/
int read(int fd, void* buf, int numBytes){

	if(!file_is_open(fd)){
		os_printf("file not open \n");
		return -1;
	}
	if (numBytes<=0)
		os_printf("Invalid number of bytes \n");

	int bytesRead =0; //this will be what we return
	// move the arguments into registers for kread to use:
	  asm volatile("mov %[fd1], r1"::[fd1]"r" (fd): "r1");
	  asm volatile("mov %[buf1], r2"::[buf1]"r" (buf): "r2");
	  asm volatile("mov %[numBytes1], r3"::[numBytes1]"r" (numBytes): "r3");

	// trigger the software_interrupt_handler in hw_handler.c:
	  asm volatile("swi SYSCALL_READ");
	// kread() gets called from within hw_handler.c

	// retrieve the return values from kread to pass back to user:

	  asm volatile("mov r1, %[bytesRead1]":[bytesRead1]"=r" (bytesRead)::);

	return bytesRead;
	//note, actual data read from file gets moved to buf in kread()
}//end open syscall

/*write() returns the number of bytes that were actually written*/
int write(int fd, void* buf, int numBytes){

	if(!file_is_open(fd)){
		os_printf("file not open \n");
		return -1;
	}
	if (numBytes<=0){
		os_printf("Invalid number of bytes\n");
		return -1;
	}
	
	int bytesWritten = 0; //this will be what we return

	// move the arguments into registers for kwrite to use:
	  asm volatile("mov %[fd1], r1"::[fd1]"r" (fd): "r1");
	  asm volatile("mov %[buf1], r2"::[buf1]"r" (buf): "r2");
	  asm volatile("mov %[numBytes1], r3"::[numBytes1]"r" (numBytes): "r3");

	// trigger the software_interrupt_handler in hw_handler.c:
	  asm volatile("swi SYSCALL_WRITE");
	// kwrite() gets called from within hw_handler.c	

	// retrieve the return values from kwrite() to pass back to user:
	  asm volatile("mov r1, %[bytesWritten1]":[bytesWritten1]"=r" (bytesWritten)::);

	return bytesWritten;
	//note, actual data written from file gets moved to buf in kwrite()
}//end write syscall

/*close() returns the 0 if successful and -1 if not*/
int close(int fd){
	int error =0; //0 if close ok, -1 if error. This will be return.
	
    if(!file_is_open(fd)){
        os_printf("file not open \n");
        return -1;
    }

	// move the arguments into registers for kclose to use:
	  asm volatile("mov %[fd1], r1"::[fd1]"r" (fd): "r1");

	// trigger the software_interrupt_handler in hw_handler.c:
	  asm volatile("swi SYSCALL_CLOSE");
	// kclose() gets called from within hw_handler.c	

	// retrieve the return values from kclose to pass back to user:
	  asm volatile("mov r1, %[error1]":[error1]"=r" (error)::);

	return error;
}//end close syscall

/*seek() returns the number of bytes that the file offset is successfully moved
  if successful, and -1 if the numBytes puts puts them outside the file length*/
int seek(int fd_int, int numBytes){
	int error =0; //-1 if numBytes + offset exceeds file length. This will be return.
	// Error checking...no need to go into kernel if numBytes is invalid:
	struct file_descriptor* fd = get_descriptor(fd_int);
	if(numBytes == 0){
		return 0;
	} else if ((numBytes > 0) && ((fd->offset + numBytes) > ((fd->linked_file)->size))) {
		os_printf("Error! file offset exceeds file size \n");
		return -1;
	}else if ((numBytes < 0) && ((fd->offset + numBytes) < 0)){
		os_printf("Error! file offset exceeds beginning of file \n");
		return -1;
	}//end if else

	// move the arguments into registers for kseek() to use:
	  asm volatile("mov %[fd1], r1"::[fd1]"r" (fd): "r1");
	  asm volatile("mov %[numBytes1], r2"::[numBytes1]"r" (fd): "r2");

	// trigger the software_interrupt_handler in hw_handler.c:
	  asm volatile("swi SYSCALL_SEEK");
	// kseek() gets called from within hw_handler.c	

	// retrieve the return values from kseek to pass back to user:
	  asm volatile("mov r1, %[error1]":[error1]"=r" (error)::);

	return error;
}//end seek syscall

/* delete() returns the 0 if successful and -1 if not */
int delete(char* filepath){
	int error =0; //0 if delete ok, -1 if error. This will be return.
	// error checking:
	// probably will have to check  if file exists in kdelete, but check here if possible...

	// move the arguments into registers for kdelete() to use:
	  asm volatile("mov %[filepath1], r1"::[filepath1]"r" (filepath): "r1");

	// trigger the software_interrupt_handler in hw_handler.c:
	  asm volatile("swi SYSCALL_DELETE");
	// kdelete() gets called from within hw_handler.c

	// retrieve the return values from kdelete to pass back to user:
	  asm volatile("mov r1, %[error1]":[error1]"=r" (error)::);

	return error;
}//end delete syscall