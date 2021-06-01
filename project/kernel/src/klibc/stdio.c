/**
 * A very barebones implementation of the stdio library from C99,
 * basically only the functions which only operate on character
 * or simple lines have been defined. This is because, at writing
 * course_os doesn't define anyway to access files so there
 * no need for a more complicated FILE interface
 */
#include <chipset.h>

int getchar(void) {
    // Get the character from the UART
    // TODO: Error handling
    return chipset.uart->getc(chipset.uart, 0);
}

char *gets(char *s) {
    char c;

    while ((c = getchar()) != '\0' || c != '\n') {
        *s++ = c;
    }

    return s;
}

int putchar(int c) {
    chipset.uart->putc(chipset.uart, c, 0);
    return c;
}
