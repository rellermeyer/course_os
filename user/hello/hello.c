#include <stdio.h>
#include <stdint.h>
#include <process.h>
#include <string.h>

uint32_t listener(uint32_t parent_tid) {
	return 0;
}

int main(int argc, char ** argv) {
	int pid = process_pid();

	printf("Process step <A>\n", pid);

	process_emit(12, 1, "Hey, how are you?", 18);

	process_yield();

	printf("Process step <B>\n", pid);

	process_emit(12, 1, "I am doing great!", 18);

	process_yield();

	printf("Process step <C>\n", pid);

	process_emit(12, 1, "Are you sure?", 18);

	process_yield();

	printf("Process step <D>\n", pid);

	process_emit(12, 1, "Yeah!!", 18);

	process_exit();
}
