#include <stdio.h>
#include <process.h>

int main() {
	//while(1);
    printf("Whats up from process!\n");
    process_yield();
    printf("Whats up from process RESUMED!!!!\n");
    process_yield();
    printf("Whats up from process RESUMED 2!!!!\n");
    while(1);
}
