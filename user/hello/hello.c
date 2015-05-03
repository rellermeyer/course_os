#include <stdio.h>
#include <process.h>

int main() {
    printf((char*)0x48);
    process_yield();
    while(1);
}
