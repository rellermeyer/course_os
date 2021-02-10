#ifndef _FLOAT_H
#define _FLOAT_H

#define SYSCALL_CREATE 0  // create a file
#define SYSCALL_SWITCH 1
#define SYSCALL_DELETE 2  // delete file or directory
#define SYSCALL_OPEN 3  // open a file
#define SYSCALL_READ 4  // read part of a file
#define SYSCALL_WRITE 5 // write part of a file
#define SYSCALL_CLOSE 6 // close a open file
#define SYSCALL_SET_PERM 7 
#define SYSCALL_MEM_MAP 8
#define SYSCALL_SEEK 9  // change the offset of an open file
#define SYSCALL_MKDIR 10  // create a direcotry
#define SYSCALL_COPY 11   // copy the contents of a file
#define SYSCALL_LS 12  // show contents of a directory


//FUNCTIONS THE USER CAN CALL TO INTERACT WITH FILESYSTEM:

int open(char* filepath, char mode);
//Opens an existing file in filepath with permissions mode. Returns the filedescriptor, or -1 if unsuccessful

int create(char* filepath, char mode);
//Creates and opens a file in filepath (end of filepath is name of the file) with permissions mode. Returns the filedescriptor, or -1 if unsuccessful

int mkdir(char* filepath);
//Creates a directory in filepath (end of filepath is name of directory). Returns -1 if unsuccessful. 

int read(int fd, void* buf, int numBytes);
//Reads numBytes bytes from the open file fd (descriptor returned by open). Returns the numeber of bytes read and fills the provided buffer, or -1 if unsuccessful

int write(int fd, void* buf, int numBytes);
//Writes numBytes bytes from the open file fd (descriptor returned by open). Returns the numeber of bytes written from the provided buffer, or -1 if unsuccessful

int close(int fd);
//Closes the open file fd (file descriptor returned by open). Returns -1 if unsuccessful

int seek(int fd, int numBytes);
//Updates the offset of an open file by numBytes. Returns -1 if unsuccessful

int delete(char* filepath);
//Deletes the file or directory specified by filepath. Returns -1 if unsuccessful

int copy(char* source, char* dest, char mode);
//Copies the contents of file or directory source into a new directory dest. The new file has permissions mode (if copying directory, put 0 as permission). Returns -1 if unsuccessful

int ls(char* filepath);
//Lists the contents of a given directory. Returns -1 if unsuccessful. 


#endif
