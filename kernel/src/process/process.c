#include <stdio.h>
#include <stdint.h>
#include <vp_array_list.h>
#include <stdlib.h>
#include <vas2.h>
#include <process.h>
#include <thread.h>
#include <elf.h>
#include <string.h>

/**
 * Create a new process.
 */
Process *create_process(Elf *elf, Process *parent) {
    Process *process = kmalloc(sizeof(Process));

    process->parent = parent;
    process->priority = DEFAULT_PROCESS_PRIORITY;
    process->vas = create_vas();
    process->threads = vpa_create(5);

    switch_to_vas(process->vas);
    for(size_t i = 0; i < elf->program.file_size; i += PAGE_SIZE) {
        allocate_page(process->vas, i, true);
    }

    memcpy((void *) elf->program.mem_location, elf->binary + elf->program.file_offset, elf->program.file_size);

    // for(int i = 0; i < elf->sections->length; i++) {
    //     ElfSection *section = vpa_get(elf->sections, i);

    //     if(memcmp(section->address, 0x00000000, 4) == 0) continue;
    // }

    Thread *thread = create_thread(elf->entry, process);
    vpa_push(process->threads, thread);

    return process;
}

/**
 * Free a process, and it's memory.
 */
void free_process(Process *process) {
    free_vas(process->vas);
    vpa_free(process->threads, (FreeFunc) free_thread);

    kfree(process);
}
