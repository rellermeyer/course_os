#ifndef UNITS_TEST_H
#define UNITS_TEST_H
#include <stdlib.h>
#include <allocator.h>
#include <stdbool.h>
#include <ansi_escape.h>

__attribute__((__common__)) size_t global_counter;

#define DEBUG_COUNT kprintf("[DEBUG COUNT] %s:%i %i\n", __FILE__, __LINE__, global_counter++);

#define TEST_CREATE(name, block)                                        \
    bool run_test_##name() { block; return TEST_PASS; }                 \
                                                                        \
    bool __internal_test_##name() {                                     \
        heap_t * heap = mem_get_allocator();                            \
        isize_t nbytes = heap->bytes_allocated;                         \
        bool res = run_test_##name();                                   \
                                                                        \
        if (res == TEST_FAIL) return TEST_FAIL;                         \
        if (nbytes != (isize_t)heap->bytes_allocated) {                 \
            ANSI_256(160);                                              \
            kprintf("├─FAILED (MEMORY LEAK: %i bytes)\n",               \
                    heap->bytes_allocated - nbytes);                    \
            ANSI(RESET);                                                \
            return TEST_FAIL;                                           \
        }                                                               \
        return TEST_PASS;                                               \
    }                                                                   \
                                                                        \
    bool test_##name() {                                                \
        ANSI_256(163);                                                  \
        puts("[TEST] ");                                                \
        ANSI(RESET);                                                    \
        kprintf("%s\n", #name);                                         \
                                                                        \
        bool res = (get_trace_memory()) ? __internal_test_##name() : run_test_##name(); \
                                                                        \
        if (res == TEST_PASS) {                                         \
            ANSI(HI_GREEN);                                             \
            kprintf("└─PASSED\n", #name);                               \
            ANSI(RESET);                                                \
            return 1;                                                   \
        } else {                                                        \
            ANSI(HI_RED);                                               \
            kprintf("└─FAILED\n", #name);                               \
            ANSI(RESET);                                                \
            return 0;                                                   \
        }                                                               \
    }
#define PASS() return TEST_PASS
#define FAIL() return TEST_FAIL
#endif
