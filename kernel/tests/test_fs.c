#include "tests.h"
#include "klibc.h"
#include "file.h"

int test_fs_1()
{
	os_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n...........................................\n");
	// Test kfind_dir real quick...
	//retval = kcreate("/", 0xFF, 1);
	//os_printf("%d\n", retval);
	// int fd = kcreate("/", 'w', 1);
	// return 0;

	// int fd2 = kcreate("/foo", 'w', 1);
	os_printf("==================================================================================");
	os_printf("\n\nCREATING /cat\n");
	int fd2_b = kcreate("/cat", 'w', 1);

	os_printf("==================================================================================");
	os_printf("\n\nCREATING /dog\n");
	int fd2_c = kcreate("/dog", 'w', 1);
	
	os_printf("==================================================================================");
	os_printf("\n\nCREATING /sham\n");
	int fd2_e = kcreate("/sham", 'w', 1);

	os_printf("==================================================================================");
	os_printf("\n\nCREATING /foo\n");
	int fd2 = kcreate("/foo", 'w', 1);

	os_printf("==================================================================================");
	os_printf("\n\nCREATING /hat\n");
	int fd2_d = kcreate("/hat", 'w', 1);

	os_printf("==================================================================================");
	// int fd2 = kcreate("/smash", 'w', 1);
	os_printf("\n\nCREATING /foo/bar.txt\n");
	int fd3 = kcreate("/foo/bar.txt", 'w', 0);

	os_printf("==================================================================================");
	os_printf("\n\nCREATING /foo/comp\n");
	int fd10 = kcreate("/foo/comp", 'w', 1);
	// int fd11 = kcreate("/foo/comp/sci", 'w', 1);
	// int fd12 = kcreate("/foo/comp/sci/lem.csv", 'w', 0);

	os_printf("=======================================================================================");
	os_printf("\n\n\nCREATING /foo/zabanga.txt\n");
	int fd4 = kcreate("/foo/zabanga.txt", 'w', 0);

	os_printf("=======================================================================================");
	os_printf("\n\n\nCREATING /foo/comp/testington.txt\n");
	int fd5 = kcreate("/foo/comp/testington.txt", 'w', 0);


	os_printf("\nDONE CREATING FILES\n");
	os_printf("=======================================================================================");

	// int fd3 = kcreate("/foo/test.txt", 'w', 0);

	// os_printf("END OF CREATE\n");
	// kclose(fd);
	kclose(fd3);
	kclose(fd2);

	kclose(fd2_b);
	kclose(fd2_c);
	kclose(fd2_d);
	kclose(fd2_e);
	kclose(fd4);
	kclose(fd5);
	// kclose(fd12);
	// kclose(fd);
	/*os_printf("\n\n*****************1\n\n");
	int fd = kcreate("/foo", 'w', 1);
	os_printf("\n\n*****************2\n\n");
	int fda = kcreate("/foo/bar.txt", 'w', 0);
	os_printf("\n\n*****************3\n\n");


	os_printf("\n\n*****************4\n\n");
	int fd4 = kcreate("/foo/dog", 'w', 1);
	os_printf("\n\n*****************5\n\n");
	int f5 = kcreate("/foo/dog/cat", 'w', 1);
	os_printf("\n\n*****************6\n\n");
	int f6 = kcreate("/foo/dog/cat/rat.txt", 'w', 0);
	os_printf("\n\n*****************7\n\n");
	// int fdb = kcreate("/foo/bar/jam.txt", 'w', 0);
	// os_printf("We have two file descriptors, fd2 is: %d fd is: %d\n", fd2, fd);
	// kclose(fdb);
	// kclose(fda);
	// kclose(fd);
	// kclose(fd2);
	kclose(fda);
	*/

//here
	os_printf("\n\n\n******************************************7\n");
	os_printf("\n a Opening file...\n");
	
	// int fd13 = kopen("/foo/comp/sci/lem.csv", 'w');
	int fd_new = kopen("/foo/bar.txt", 'w');
	int fd_new2 = kopen("/foo/comp/testington.txt", 'w');
	os_printf("file descriptor is: %d\n", fd_new);
	os_printf("\nWriting string to file...\n");
	char *s = "Hello, worloooooooooooood I'm JJJKLKKKKKKKKKUUUUUU testing right now...!";
	char *t = "HELLO IM TESTING THE lemons.csv file! here is lotttttttttttttttttttttttttttttttttttttssssssssssss ooooooofffffffff tttttttttteeeeeeexxxxxxttttt";
	kwrite(fd_new, s, os_strlen(s));
	kwrite(fd_new2, t, os_strlen(t));
	// kwrite(fd13, t, os_strlen(t));
	kclose(fd_new);
	kclose(fd_new2);
	// kclose(fd13);
	// while(1);

	// Okay, now we should be able to open it.
	os_printf("\neeeeee Opening previous file...\n");
	int fd_znew = kopen("/foo/bar.txt", 'r');
	int fd_znew2 = kopen("/foo/comp/testington.txt", 'r');
	// int fd_g = kopen("/foo/comp/sci/lem.csv", 'w');

	// And read from it
	os_printf("\nReading from file...\n");
	char buf[256];
	os_memset(&buf, 0, 256);
	int nbytes = kread(fd_znew, buf, 256);
	os_printf("Read %d bytes from file.\n", nbytes);
	os_printf("the buffer is: '%s'\n", buf);
	kclose(fd_znew);
	
	os_printf("\nReading from file2...\n");
	char buf2[256];
	os_memset(&buf2, 0, 256);
	nbytes = kread(fd_znew2, buf2, 256);
	os_printf("Read %d bytes from file.\n", nbytes);
	os_printf("the buffer is: '%s'\n", buf2);
	kclose(fd_znew2);


//here
	// Test kls, just for grins. :)
	//os_printf("\nRunning kls...\n");
	//kls("/"); */

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