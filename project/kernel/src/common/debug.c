/**
 * debug.
 *
 * Author: Valentijn van de Beek, Zohar Cochavi, Atanas Pashov
 *
 * Added: Spring 2021
 * Last modified: Spring 2021

 * Purpose:
 * This file defines a debugging shell including an API hook to add
 * new commands to the shell easily.
 */
#include <allocator.h>
#include <chipset.h>
#include <debug.h>
#include <hardwareinfo.h>
#include <interrupt.h>
#include <klibc.h>
#include <string.h>
#include <test.h>

VPSinglyLinkedList *commands;
static bool debug_running = true;


/***********************************************************/
/* Commands which apply to parts of the kernel before the  */
/* initialization of virtual memory. This is due the fact  */
/* that the debug shell has a dependency on kmalloc        */
/***********************************************************/
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


/**************************************************************/
/* Initialize the the command list and popuilate with some    */
/* good initial values.                                       */
/**************************************************************/
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


/*******************************************************************************/
/* Create the data structure to store a single command.                        */
/* name: a string containing the command name                                  */
/* func: a function for the code that the command should execute. The          */
/* function should return a void and take a single linked list as argument.    */
/*******************************************************************************/
struct command *debug_create_command(char *name, void (*func) (VPSinglyLinkedListIterator args)) {
    struct command *cmd = kmalloc(sizeof(struct command));
    size_t len = strlen(name);
    cmd->name = kmalloc(len);
    strcpy(cmd->name, name);
    cmd->name[len] = '\0';
    cmd->command = func;
    return cmd;
}


/***************************************/
/*  Add a command to the debug shell   */
/*  cmd: a command structure.          */
/***************************************/
void debug_add_command(struct command *cmd) {
    vpsll_push(commands, cmd);
}


/*************************************************/
/* An internal function for the freeing function */
/*************************************************/
void debug_free_internal(void *data) {
    struct command *cmd = data;
    kfree(cmd->name);
    kfree(cmd);
}


/******************************************/
/* Cleanup the memory for the debug shell */
/******************************************/
void debug_free(void) {
    vpsll_free(commands, debug_free_internal);
}


/*****************************************************************/
/* A small helper for the comparing function of the linked list  */
/*****************************************************************/
bool debug_cmp_internal(void *in, void *other) {
    if (in == NULL)
        return false;

    struct command *cmd = in;
    return strcmp(cmd->name, (char*) other) == 0;
}

/***************************/
/* Actually runs the shell */
/***************************/
void debug_run(void) {
    char buf[9001];
    int i = 0;

    puts("> ");
    while (debug_running) {
        buf[i] = chipset.uart->getc(chipset.uart, 0);
        kprintf("%c", buf[i]);

        // Find the deletion character
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
            kprintf("%c", '\n');

            // Split the the input into words so we can process it easier
            char *tokens[1] = {" "};
            VPSinglyLinkedList *words = ktokenize(buf, tokens, 1);

            // If it empty don't look anything up
            if (vpsll_length(words) == 0) {
                vpsll_free(words, kfree);
                continue;
            }

            // Find the command in the linked list
            char *cmd_name = vpsll_pop(words);
            struct command *cmd = vpsll_find(commands, cmd_name, debug_cmp_internal);
            kfree(cmd_name);

            // Run the command with the tracing if set.
            if (cmd != NULL) {
                set_trace_memory(trace);
                cmd->command(vpslli_create(words));
                trace = get_trace_memory();
            } else {
                kprintf("Command '%s' not found.", buf);
            }

            // Cleanup after ourselves without tracing
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
