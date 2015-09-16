/**
 * @file
 *
 * Build FS (Bootstraps the File System)
 *
 * @author  Lane Kolbly, Ginevra Gaudioso
 * @version 1.0
 *
 * @section NOTE ON CMDLINE FOLDER
 * SD card gets run automatically in the makefile, however
 * the .h files in the cmdline folder need to be manually updated and
 * are independent of the files in the include folder
 * > e.g. if someone changes include/file.h then fs/cmdline/file.h 
 *   needs to be updated
 *
 * POSSIBLE TODO: Somehow consolodate include files into one directory
 * 
 * @section DESCRIPTION
 * 
 * Takes a directory and creates an SD card image from
 * it
 *
 */
#include "file.h"
#include<stdio.h>
#include<dirent.h>
#include<string.h>

/**
 * Adds a specific file to the card.sd image
 *
 * Adds the file at the path specified by filename to the SD card
 * image indicated by dst_filename
 *
 * @param 
 *
 * char *filename - holds the path to the file to add to the SD
 * card image
 *
 * @param
 *
 * char *dst_filename - holds the path to the SD card image to
 * add the file to
 */
void addFile(char *filename, char *dst_filename)
{
	FILE *f = fopen(filename, "r");
	kclose(kcreate(dst_filename, 'r', 0));
	int fd = kopen(dst_filename, 'w');
	char buf[4096];
	int nread;
	while ((nread=fread(buf, 1, 4096, f)) > 0) {
		kwrite(fd, buf, nread);
	}
	kclose(fd);
	fclose(f);
}

int main(int argc, char **argv)
{
	int fd;

	kfs_init(0,0,1);

	// Create a file, I guess?
	kclose(kcreate("/foobar", 'r', 0));

	// Write to the file
	fd = kopen("/foobar", 'w');
	char *s = "Hello, world!";
	kwrite(fd, s, strlen(s));
	kclose(fd);

	// Read from the file
	fd = kopen("/foobar", 'r');
	char buf[256];
	kread(fd, buf, 255);
	kclose(fd);
	printf("Read: '%s'\n", buf);

	// Let's iterate through the directory ./data/
	DIR *dir = opendir(argv[1]);
	if (dir == 0) {
		printf("SAD :(\n");
		return -1;
	}
	struct dirent *entry;
	while ((entry = readdir(dir))) {
		if (entry->d_name[0] == '.') continue; // skip
		printf("Adding %s...\n", entry->d_name);
		char buf[1024];
		char buf2[1024];
		snprintf(buf, 1024, "%s/%s", argv[1], entry->d_name);
		snprintf(buf2, 1024, "/%s", entry->d_name);
		addFile(buf, buf2);
	}
	closedir(dir);

	// Try reading something....
	fd = kopen("/hello", 'r');
	printf("fd: %d\n", fd);
	kclose(fd);
	return 0;
}
