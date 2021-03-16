#include <allocator.h>
#include <chipset.h>
#include <debug.h>
#include <hardwareinfo.h>
#include <interrupt.h>
#include <klibc.h>
#include <string.h>
#include <test.h>

VPSinglyLinkedList *commands;
bool debug_running = true;

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

void shutdown_command(VPSinglyLinkedListIterator args) {
    SemihostingCall(ApplicationExit);
}

void exit_debug(VPSinglyLinkedListIterator args) {
    debug_running = false;
}

void hardwareinfo_command(VPSinglyLinkedListIterator args) {
    print_hardwareinfo();
}

void echo_command(VPSinglyLinkedListIterator args) {
    while (!vpslli_empty(args)) {

        kprintf("%s ", vpslli_next(&args));
    }
}

void malloc_command(VPSinglyLinkedListIterator args) {
    // Ignore all other arguments
    for (int i = 0; i < 10; i++) {
        kfree(kmalloc(2000));
    }
}

void trace_command(VPSinglyLinkedListIterator args) {
    set_trace_memory(!get_trace_memory());
    if (get_trace_memory()) {
        puts("Enabled memory tracing");
    } else {
        puts("Disable memory tracing");
    }
}

void debug_init(void) {
    commands = vpsll_create();
    // Disable the memory tracing for a bit
    const bool trace = get_trace_memory();
    set_trace_memory(false);
    debug_add_command(debug_create_command("panic", panic_command));
    debug_add_command(debug_create_command("test", test_command));
    debug_add_command(debug_create_command("shutdown", shutdown_command));
    debug_add_command(debug_create_command("hardwareinfo", hardwareinfo_command));
    debug_add_command(debug_create_command("echo", echo_command));
    debug_add_command(debug_create_command("malloc", malloc_command));
    debug_add_command(debug_create_command("trace", trace_command));
    debug_add_command(debug_create_command("exit", exit_debug));
    set_trace_memory(trace);
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
    while (debug_running) {
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
            // Store the old trace memory value
            bool trace = get_trace_memory();
            set_trace_memory(false);
            // Skip trailing spaces for consitency
            while (buf[i - 1] == ' ') {
                i--;
            }


            buf[i] = '\0';
            kprintf("%c", '\n');                \
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
                set_trace_memory(trace);
                cmd->command(vpslli_create(words));
                trace = get_trace_memory();
            } else {
                kprintf("Command '%s' not found.", buf);
            }
            set_trace_memory(false);
            vpsll_free(words, kfree);
            set_trace_memory(trace);

            puts("\n> ");
            i = 0;
        } else {
            i++;
        }
    }

}
