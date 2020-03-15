int main() {
    asm volatile(
        "mov r7, #4 \n" \
        "swi $0 \n"
    );
    return 0;
}

