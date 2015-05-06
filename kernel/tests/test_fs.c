#include "tests.h"
#include "klibc.h"
#include "../include/file.h"

int test_fs_1()
{
	os_printf("\n\n\n...........................................\n");


	os_printf("\ncreating file in root -----------------------------------------\n");
	int fd2 = kcreate("/file", 'w', 0);
	os_printf("fd returned by creating file %d -----------------------------------------\n", fd2);
	kls("/");

	os_printf("closing file -----------------------------------------\n");
	int error = kclose(fd2);
	os_printf("error from closing = %d \n", error);

	os_printf("\n\n\n...........................................\n");

	os_printf("\n\nCreating folder -----------------------------------------\n");

	int fd1 = kcreate("/folder", 'w', 1);
	os_printf("fd returned by creating folder %d -----------------------------------------\n", fd1);

	os_printf("\ncreating file -----------------------------------------\n");
	int fd3 = kcreate("/folder/file", 'w', 0);
	os_printf("fd returned by creating file %d -----------------------------------------\n", fd2);

	os_printf("closing file -----------------------------------------\n");
	kclose(fd3);





	// os_printf("\nOpening file...\n");
	// fd = kopen("/foo/bar.txt", 'w');
	// os_printf("file descriptor is: %d\n", fd);
	// os_printf("\nWriting string to file...\n");
	// char *s = "Hello, world I'm testing right now...!\n";
	// kwrite(fd, s, os_strlen(s));
	// kclose(fd);
	// //while(1);

	// // Okay, now we should be able to open it.
	// os_printf("\nOpening previous file...\n");
	// fd = kopen("/foo/bar.txt", 'r');

	// // And read from it
	// os_printf("\nReading from file...\n");
	// char buf[256];
	// int nbytes = kread(fd, buf, 256);
	// os_printf("Read %d bytes from file.\n", nbytes);
	// os_printf("the buffer is: '%s'\n", buf);

	// kclose(fd);

	// // Test kls, just for grins. :)
	// //os_printf("\nRunning kls...\n");
	// //kls("/");

	return 0;
}

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
	struct stats * result;
	get_stats("/foobar2", result);
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
