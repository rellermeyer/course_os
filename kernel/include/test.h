#ifndef TEST_H
#define TEST_H

// Each test consists of its name and a 
// function pointer that is to be tested
typedef struct TEST {
	char* test_name;
	int   (*ptr_to_test)();
} TEST;

TEST create_test(char *name, int (*function_to_test)());

#endif
