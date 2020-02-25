#include "tests.h"
#include "klibc.h"
#include "fs/file.h"



int test_fs_1()
{
	os_printf("\nCREATING /foo\n");
	int fd1 = kcreate("/foo", 'w', 1);

	os_printf("\nCREATING /bar\n");
	int fd2 = kcreate("/bar", 'w', 1);

	os_printf("\nCREATING /foo/baz.txt\n");
	int fd3 = kcreate("/foo/baz.txt", 'w', 0);

	os_printf("closing /foo/baz.txt \n");
	kclose(fd3);
	
	os_printf("\nnow opening file: /foo/baz.txt\n");
	int fd_new = kopen("/foo/baz.txt", 'w');
	os_printf("file descriptor is: %d\n", fd_new);
	os_printf("closing /foo/baz.txt \n");
	kclose(fd_new);

	os_printf("\nnow opening file: /foo/baz.txt again to write to it\n");
	fd_new = kopen("/foo/baz.txt", 'w');
	char *s = "Hellooooooooooooooooooooooolllllllllllll world!!!";
	kwrite(fd_new, s, os_strlen(s));
	kclose(fd_new);
	os_printf("closing /foo/baz.txt \n");

	os_printf("\nnow opening file: /foo/baz.txt to read from it\n");
	fd_new = kopen("/foo/baz.txt", 'r');
	char buf[256];
	os_memset(&buf, 0, 256);
	int nbytes = kread(fd_new, buf, 256);
	os_printf("Read %d bytes from file.\n", nbytes);
	os_printf("the buffer is: '%s'\n", buf);
	kclose(fd_new);
	kclose (fd1);
	kclose (fd2);
	kclose (fd3);

	return 0;
}//end test_fs_1()

int test_fs_1_old()
{	

	return 0;
}//end test_fs1()




int test_fs_2() {
	LOG("\nWriting a lot of data to /foobar2...\n");

	int fd = kcreate("/foobar2", 'w', 0);
	kclose(fd);

	// Okay, let's try making /foobar really big.
	fd = kopen("/foobar2", 'w');
	int buf[256];
	int i;
	for (i=0; i<256; i++) {
		buf[i] = i*3;
	}
	// Write buf out 128 times
#define CNT 64
	for (i=0; i<CNT; i++) {
		kwrite(fd, buf, sizeof(int)*256);
	}
	// 128K should be in the file
	kclose(fd);

	LOG("\nReading a lot of data from /foobar2...\n");
	fd = kopen("/foobar2", 'r');
	for (i=0; i<CNT; i++) {
		os_memset(buf, 0, sizeof(int)*256);
		kread(fd, buf, sizeof(int)*256);
		int j;
		for (j=0; j<256; j++) {
			if (buf[j] != j*3) {
				os_printf("%d: %d, expect %d\n", j, buf[j], j*3);
				return -1;
			}
		}
		//os_printf("%d is valid.\n", i);
	}
	kclose(fd);
	struct stats * result = NULL;
	get_stats("/foobar2", result);
	assert(!result->is_dir);
	return 0;
}

void run_fs_tests() {
 	Test *tests[1];
 	tests[0] = create_test("test_fs_1", &test_fs_1);
	//tests[1] = create_test("test_fs_2", &test_fs_2);
	//run_tests(tests, 2);
	// tests[1] = create_test("test_fs_2", &test_fs_2);
	run_tests(tests, 1);
}
