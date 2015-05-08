#include "file.h"
#include<stdio.h>
#include<dirent.h>
#include<string.h>

void addFile(char *filename, char *dst_filename)
{/*
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
	*/
}

int main(int argc, char **argv)
{/*
	kfs_init(0,0,1);

	// Create a file, I guess?
	kclose(kcreate("/foobar", 'r', 0));

	// Write to the file
	int fd = kopen("/foobar", 'w');
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
		char buf[256];
		char buf2[256];
		snprintf(buf, 256, "%s/%s", argv[1], entry->d_name);
		snprintf(buf2, 256, "/%s", entry->d_name);
		addFile(buf, buf2);
	}
	closedir(dir);

	// Try reading something....
	fd = kopen("/hello", 'r');
	printf("fd: %d\n", fd);
	kclose(fd);
*/
	return 0;

	
}
