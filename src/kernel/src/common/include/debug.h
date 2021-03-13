#pragma once
#ifndef DEBUG_H

#define DEBUG_H
#include <ds.h>

struct command {
    char *name;
    void (*command)(VPSinglyLinkedListIterator args);
};

struct command *debug_create_command(char *name, void (*func) (VPSinglyLinkedListIterator args));

void debug_init(void);
void debug_add_command(struct command *new);
void debug_run(void);
void debug_delete(void);

#endif
