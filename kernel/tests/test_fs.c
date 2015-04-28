#include "tests.h"
#include "klibc.h"
#include "file.h"

int test_fs_1()
{
	// Test kfind_dir real quick...
	struct dir_helper dh;
	kfind_dir("/", &dh);
	kfind_dir("/etc", &dh);
	kfind_dir("/etc/", &dh);
	kfind_dir("/etc/fstab", &dh);

	//retval = kcreate("/", 0xFF, 1);
	//os_printf("%d\n", retval);
	int fd = kcreate("/foobar", 'w', 0);
	os_printf("%d\n", fd);
	kclose(fd);

	os_printf("\nOpening file...\n");
	fd = kopen("/foobar", 'w');
	os_printf("\nWriting string to file...\n");
	char *s = "Hello, world!\n";
	kwrite(fd, s, os_strlen(s));
	kclose(fd);
	//while(1);

	// Okay, now we should be able to open it.
	os_printf("\nOpening previous file...\n");
	fd = kopen("/foobar", 'r');

	// And read from it
	os_printf("\nReading from file...\n");
	char buf[256];
	int nbytes = kread(fd, buf, 256);
	os_printf("Read %d bytes from file.\n", nbytes);
	os_printf("'%s'\n", buf);

	kclose(fd);

	// Test kls, just for grins. :)
	os_printf("\nRunning kls...\n");
	kls("/");

	return 0;
}

int test_fs_2() {
	LOG("\nWriting a lot of data to /foobar...\n");

	// Okay, let's try making /foobar really big.
	int fd = kopen("/foobar", 'w');
	int buf[256];
	int i;
	for (i=0; i<256; i++) {
		buf[i] = i*3;
	}
	// Write buf out 128 times
	for (i=0; i<128; i++) {
		kwrite(fd, buf, sizeof(int)*256);
	}
	// 128K should be in the file
	kclose(fd);

	LOG("\nReading a lot of data from /foobar...\n");
	fd = kopen("/foobar", 'r');
	for (i=0; i<128; i++) {
		kwrite(fd, buf, sizeof(int)*256);
		int j;
		for (j=0; j<256; j++) {
			if (buf[j] != j*3) {
				return -1;
			}
		}
	}
	kclose(fd);

	return 0;
}

void run_fs_tests() {
	Test *tests[2];
	tests[0] = create_test("test_fs_1", &test_fs_1);
	tests[1] = create_test("test_fs_2", &test_fs_2);
	run_tests(tests, 2);
}
