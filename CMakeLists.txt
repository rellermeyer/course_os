########################################################################
## WARNING: This cmake file does *NOT* work to compile the course OS  ##
## It is merely here to make clion detect all the source files        ##
## so it can do code inspection. Use the supplied makefile to compile ##
########################################################################

cmake_minimum_required(VERSION 3.6)

project(course_os)

set(SOURCE_FILES
    kernel/argparse.c
    kernel/data_structures/array_list.c
    kernel/data_structures/bin_tree.c
    kernel/data_structures/bitvector.c
    kernel/data_structures/hash_map.c
    kernel/data_structures/linked_list.c
    kernel/data_structures/priority_queue.c
    kernel/data_structures/ring_buffer.c
    kernel/data_structures/swap_pqueue.c
    kernel/drivers/clock.c
    kernel/drivers/mmci.c
    kernel/drivers/timer.c
    kernel/drivers/uart.c
    kernel/drivers/uart_pi.c
    kernel/elf.c
    kernel/fs/cmdline/fakelibs.c
    kernel/fs/cmdline/main.c
    kernel/fs/fat16/test.c
    kernel/fs/fat16/file.c
    kernel/fs/open_table.c
    kernel/hw_handlers.c
    kernel/interrupt.c
    kernel/klibc.c
    kernel/loader.c
    kernel/memory/allocator.c
    kernel/memory/mem_alloc.c
    kernel/mmap.c
    kernel/pi_light.c
    kernel/pm.c
    kernel/priorityQueue.c
    kernel/process.c
    kernel/process/process_api.c
    kernel/process/scheduler.c
    kernel/signals.c
    kernel/tests.c
    kernel/tests/test_fs.c
    kernel/tests/test_hash_map.c
    kernel/tests/test_mem_alloc.c
    kernel/tests/test_priority_queue.c
    kernel/tests/test_vm.c
    kernel/tests/testingsuite_example/arrayfill/arrayfill.c
    kernel/tests/testingsuite_example/arrayfill/tap.c
    kernel/tests/testingsuite_example/arrayfill/testarrayfill.c
    kernel/threads/kthreads.c
    kernel/vm/fastlz/6pack.c
    kernel/vm/fastlz/6unpack.c
    kernel/vm/fastlz/fastlz.c
    kernel/vm/frame.c
    kernel/vm/swap_framework.c
    kernel/vm/swap_fs.c
    kernel/vm/vm.c
    kernel/start.c
)

add_executable(course_os ${SOURCE_FILES})

include_directories(kernel/include)
