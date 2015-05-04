#include <stdio.h>
#include <malloc_syscalls.h>
int main() {
	int k = BUFSIZ;
	int *m = malloc(2*BUFSIZ);
	free(m);
    printf("Hello world... from hello.c\n");
    
    while(1);
}