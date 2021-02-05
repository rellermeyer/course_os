int main() {
    for (;;) {
        asm volatile("ldr r1, =0x80008000 \n"
                     "mov r2, #42 \n"
                     "ldr r2, [r1] \n"
                     "mov r7, #4 \n"
                     "swi $0 \n");
    }
    return 0;
}
