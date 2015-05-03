#include <stdio.h>
#include <process.h>

int main() {
    printf("Whats up from process!\n");
    process_yield();
    while(1);
}
