#include "file.h"

int main(int argc, char **argv)
{
	kfs_init(0,0);

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

	return 0;
}
