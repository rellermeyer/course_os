#include <chipset.h>
#include <string.h>
#include <klibc.h>
#include <test.h>
#include <interrupt.h>
#include <debug.h>
#include <hardwareinfo.h>

VPSinglyLinkedList *commands;

void panic_command(VPSinglyLinkedListIterator args) {
    panic();
}

void test_command(VPSinglyLinkedListIterator args) {
    #ifdef ENABLE_TESTS
    test_main();
    #else
    puts("Tests disabled");
    #endif
}

void exit_command(VPSinglyLinkedListIterator args) {
    SemihostingCall(ApplicationExit);
}

void hardwareinfo_command(VPSinglyLinkedListIterator args) {
    print_hardwareinfo();
}

void echo_command(VPSinglyLinkedListIterator args) {
    while (!vpslli_empty(args)) {

        kprintf("%s ", vpslli_next(&args));
    }
}

void debug_init(void) {
    commands = vpsll_create();

    debug_add_command(debug_create_command("panic", panic_command));
    debug_add_command(debug_create_command("test", test_command));
    debug_add_command(debug_create_command("exit", exit_command));
    debug_add_command(debug_create_command("hardwareinfo", hardwareinfo_command));
    debug_add_command(debug_create_command("echo", echo_command));
}

struct command *debug_create_command(char *name, void (*func) (VPSinglyLinkedListIterator args)) {
    struct command *cmd = kmalloc(sizeof(struct command));
    size_t len = strlen(name);
    cmd->name = kmalloc(len);
    strcpy(cmd->name, name);
    cmd->name[len] = '\0';
    cmd->command = func;
    return cmd;
}

void debug_add_command(struct command *cmd) {
    vpsll_push(commands, cmd);
}

void debug_free_internal(void *data) {
    struct command *cmd = data;
    kfree(cmd->name);
    kfree(cmd);
}

void debug_free(void) {
    vpsll_free(commands, debug_free_internal);
}

bool debug_cmp_internal(void *in, void *other) {
    if (in == NULL)
        return false;

    struct command *cmd = in;
    return strcmp(cmd->name, (char*) other) == 0;
}

void debug_run(void) {
    char buf[9001];
    int i = 0;

    puts("> ");
    while (true) {
        buf[i] = chipset.uart->getc(chipset.uart, 0);
        kprintf("%c", buf[i]);

        if (buf[i] == 127) {
            if (i == 0)
                continue;

            // Go back, overwrite the character with a space and go back again
            puts("\b \b");
            buf[i] = '\0';
            i--;
        } else if (buf[i] == 13) {
            // Skip trailing spaces for consitency
            while (buf[i - 1] == ' ') {
                i--;
            }

            buf[i] = '\0';
            kprintf("%c", '\n');
            char *tokens[1] = {" "};
            VPSinglyLinkedList *words = ktokenize(buf, tokens, 1);

            if (vpsll_length(words) == 0) {
                vpsll_free(words, kfree);
                continue;
            }

            // Find the command in the linked list
            char *cmd_name = vpsll_pop(words);
            struct command *cmd = vpsll_find(commands, cmd_name, debug_cmp_internal);
            kfree(cmd_name);


            if (cmd != NULL) {
                cmd->command(vpslli_create(words));
            } else {
                kprintf("Command '%s' not found.", buf);
            }

            vpsll_free(words, kfree);

            puts("\n> ");
            i = 0;
        } else {
            i++;
        }
    }

}
