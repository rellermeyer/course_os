#include <stdint.h>
#include "global_defs.h"
#include "argparse.h"
#include "klibc.h"
#include "tests.h"
#include "process.h"
#include "file.h"


static void argparse_parse(char *);

/* Get the command-line arguments and run the functions to process them. */
void argparse_process(uint32_t *p_bootargs)
{
	for (atag_iterator(tag, p_bootargs))
	{
		os_printf("tag (+%d) %d\n",
				(((uint32_t) ((uint32_t*) tag)) - ((uint32_t) p_bootargs)),
				tag->header.tag);
		atag_print(tag);
		if (tag->header.tag == ATAG_CMDLINE)
		{
			argparse_parse((char*) &tag->content.cmdline.cmdline);
		}
	}
}

void atag_print(struct atag *t)
{
	switch (t->header.tag)
	{
	case ATAG_CORE:
		if (t->header.size == 2)
		{
			os_printf("ATAG_CORE\n");
		}
		else
		{
			os_printf("ATAG_CORE (FLAGS=%d, PAGESIZE=%d, ROOTDEV=%d)\n",
					t->content.core.flags, t->content.core.pagesize,
					t->content.core.rootdev);
		}
		break;
	case ATAG_MEM:
		os_printf("ATAG_MEM (SIZE=%d, START=%d)\n", t->content.mem.size,
				t->content.mem.start);
		break;
	case ATAG_CMDLINE:
		os_printf("ATAG_CMDLINE (%s)\n", &t->content.cmdline.cmdline);
		break;
	}
}

/*
	Pulls name, start location, and size of file from command line
	Opens file for process, then moves the process from
	file to a location in memory.
	Creates a process using the start location of process,
	enables the VAS for the process, sets up the stack and heap,
	then executes it.
	@param Character pointer to command line
	@param char* cmdline

*/

static void argparse_parse(char *cmdline)
{

	char* token = os_strtok(cmdline, " ");

	while (token != NULL)
	{
		os_printf("token: %s\n", token);

		if (os_strcmp("-load", token) == 0)
		{
			char* name = os_strtok(NULL, " ");
			uint32_t start = string_to_unsigned_int(os_strtok(NULL, " "), 16);
			uint32_t len = string_to_unsigned_int(os_strtok(NULL, " "), 16);



			os_printf("LOADING PROCESS <<%s>>, start address %X, length %X\n",
					name, start, len);


			os_printf("START: %X \n", start+PROC_LOCATION);

			int fd = kopen("/hello", 'r');
			struct stats fstats;
			start += PROC_LOCATION;
			get_stats("/hello", &fstats);
			 len = fstats.size;

			int* location = start;
			int storage2 = 0;
			
			for(int i = 0; i < (len/BLOCK_SIZE) + 1; i++){
				os_printf("argparse.c: allocating page %d of %d\n", i+1, (len/BLOCK_SIZE) + 1);
				uint32_t *v = start + (i* BLOCK_SIZE);
				int x = vm_allocate_page(KERNEL_VAS, (void*)v, VM_PERM_USER_RW );		
				
			}

			int counter =0;
			while(counter < len){
				kread(fd, location, 4);
				location +=1;
				counter +=4;
				
			}
			//assert(1==12);
			
			pcb *test= process_create((uint32_t*) start);

				vm_enable_vas(test->stored_vas);

			test->len = len;
			test->start = start;
			test->name = name;
			setup_process_vas(test);
			init_proc_stack(test);
			init_proc_heap(test);
			
			for(int i = 0; i < (len/BLOCK_SIZE) + 1; i++){
				uint32_t *v = start + (i* BLOCK_SIZE);
				vm_free_mapping(KERNEL_VAS, (void*)v);		
				
			}

			execute_process(test);
		}
		else if (os_strcmp("-test", token) == 0)
		{
			os_printf("RUNNING TESTS\n");
			os_printf("Running tests...\n");
			Test *tests[2];
			tests[0] = create_test("This passes", &test1);
			tests[1] = create_test("This fails", &test2);
			run_tests(tests, 2);
		}

		token = os_strtok(NULL, " ");
	}
}

/* We return a signed integer because we need a way to detect errors.

 Read a string of characters and interpret them as an unsigned integer.
 Return -1 if the string cannot be read as an unsigned integer.
 Return an integer representing the value of the string otherwise.
 */
int string_to_unsigned_int(char *input, int base)
{
	int i = os_strlen(input) - 1; // Index in the string

	if (hex_value_of_character(input[i]) == -1)
	{
		// Return -1 if the string cannot be read as an unsigned integer
		return -1;
	}

	int n = 0; // Number of digits
	int result = 0;

	// Starting from the end of the string, read each character
	while (i >= 0)
	{
		// Get the value of one character
		int digit = hex_value_of_character(input[i]);

		if (digit == -1)
		{
			// If we hit a non-digit character such as 'x', return
			return result;
		}

		// Add the digit's value to the integer
		result = result | (digit << (n * 4));

		i--;
		n++;
	}
	return result;
}

/* Return the hexidecimal value of the single character c */
int hex_value_of_character(char c)
{
	if (c >= '0' && c <= '9')
	{
		return (int) (c - '0');
	}
	else if (c >= 'A' && c <= 'F')
	{
		return (int) (c - 'A' + 10);
	}
	else if (c >= 'a' && c <= 'f')
	{
		return (int) (c - 'a' + 10);
	}

	// Return -1 if c was not a hex digit
	return -1;
}


uint32_t round_up(uint32_t c){


	c--;
	c |= c >> 1;

    c |= c >> 2;
    c |= c >> 4;
    c |= c >> 8;

    c |= c >> 16;
    c++;
    return c;

}