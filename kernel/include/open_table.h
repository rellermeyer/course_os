#ifndef __OPEN_TABLE_H__
#define __OPEN_TABLE_H__

//header for the open table file
//that implements the open table for the file system

//please use provided functinos to add/remove items 


void fs_table_init(); //initializes fs open table at boot up time

int add_to_opentable(struct file * f);  //adds a file to the opentable
					//returns fd if successul
					//and -1 if unsuccessful

int delete_from_opentable(int fd);      //deletes a file from the opentable
					//returns 0 if successful
					//and -1 if unsuccessful

int file_is_open(int fd); //checks if there is an entry corrensponding to that fd


//cleanup functinos to be implemented when shutdown will be implemented

#endif 
