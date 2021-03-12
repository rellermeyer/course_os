#include <chipset.h>
#include <string.h>
#include <klibc.h>
#include <test.h>
#include <interrupt.h>
#include <debug.h>

void run_debug(void) {
    char buf[9001];
    int i = 0;

    puts("> ");
    while (true) {
        buf[i] = chipset.uart->getc(chipset.uart, 0);
        kprintf("%c", buf[i]);

        if (buf[i] == 127) {
            // Go back, overwrite the character with a space and go back again
            puts("\b \b");
            buf[i] = '\0';
            i--;
        } else if (buf[i] == 13) {
            // Induce a kernel panic
            buf[i] = '\0';
            kprintf("%c", '\n');
            #ifdef ENABLE_TESTS
            if (strcmp(buf, "panic") == 0) {
                panic();
            } else if (strcmp(buf, "test") == 0) {
                test_main();
            } else if (strcmp(buf, "exit") == 0) {
                SemihostingCall(ApplicationExit);
            }
            #endif

            kprintf("%s\n> ", buf);
            i = 0;
        } else {
            i++;
        }
    }

}
