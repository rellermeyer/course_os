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
 * 
 * @section DESCRIPTION
 * 
 * Takes a directory and creates an SD card image from
 * it
 *
 */
#include "fs/file.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>

#define BUF_SIZE 4096
#define MAX_FILE_NAME_LEN 1024

/**
 * Adds a specific file to the card.sd image
 *
 * Adds the file at the path specified by filename to the SD card
 * image indicated by dst_filename
 *
 * @param 
 *
 * char *src_file_name - holds the path to the file to add to the SD
 * card image
 *
 * @param
 *
 * char *dst_file_name - holds the path to the SD card image to
 * add the file to
 */
void addFile(const char *src_file_name, const char *dst_file_name)
{
	FILE *f = fopen(src_file_name, "r");
	assert(f);

	// FIXME: should all be const char*
	kclose(kcreate((char*) dst_file_name, 'r', 0));
	int fd = kopen((char*) dst_file_name, 'w');

	char buf[BUF_SIZE];
	int nread;

	while ((nread = fread(buf, 1, BUF_SIZE, f)) > 0)
	{
		kwrite(fd, buf, nread);
		//printf("\t\t%d bytes\n", nread);
	}

	kclose(fd);
	fclose(f);
}

int main(int argc, char **argv)
{
	int fd;
	int i;
	int l;
	DIR *dir;
	struct dirent *entry;

	char src_file[MAX_FILE_NAME_LEN];
	char dst_file[MAX_FILE_NAME_LEN];

	kfs_init(0, 0, 1);

	for (i = 1; i < argc; i++)
	{
		dir = opendir(argv[i]);
		if (dir == NULL)
		{
			printf("Could not find directory %s\n", argv[i]);
			printf("SAD :(\n");
			return -1;
		}
		printf("In %s\n", argv[i]);

		while ((entry = readdir(dir)))
		{
			if (entry->d_name[0] == '.')
				continue; // skip

			l = snprintf(src_file, MAX_FILE_NAME_LEN, "%s/%s", argv[i], entry->d_name);
			assert(l < MAX_FILE_NAME_LEN);
			l = snprintf(dst_file, MAX_FILE_NAME_LEN, "/%s", entry->d_name);
			assert(l < MAX_FILE_NAME_LEN);

			printf("\tAdding %s to %s ...\n", src_file, dst_file);

			addFile(src_file, dst_file);
		}

		closedir(dir);
	}

	return 0;
}
