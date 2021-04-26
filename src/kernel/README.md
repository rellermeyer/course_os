# Running the kernel

The [makefile](Makefile) in this directory contains a number of configuration options.

| Configuration opion | description |
| --- | --- |
| CFLAGS | Flags given to the c compiler |
| MEMORY | The amount of memory given to qemu. Until memory detection works, must be `1G` |
| DEFINITIONS | These are variables given to the c preprocessor. Options for these are listed below. |
| KERNEL_PARAMS | Currently not supported and outdated |
| SOURCEDIR | The name of the directory containing the sourcecode. |
| BUILDDIR | The name of the directory containing object files and binaries |
| INCLUDEDIR | Any directory in SOURCEDIR with this name, will be globally included in every c file. This means they can be included with `#include <something.h>` instead of `#include "something.h"`. |
| TEST_MAIN_FILE | The name of the file generated to contain all [tests](src/test/README.md). | 
| TESTS_SEED | The seed used to randomize the order in which the tests are run. |
| CPU | The cpu type emulated by qemu. Supported cpu types are the `arm1176` and `cortex-a7`|


## Definitions

| Definition | Action |
| --- | --- |
| ENABLE_TESTS | With this definition enabled, tests are compiled into the kernel |
| MEM_DEBUG | Every kernel heap memory allocation (kmalloc, kfree) will be logged at TRACE level. Tests will be checked for memory leaks. |
| LOG_LEVEL | Number between 1 and 4 describing the log level of the kernel. 1 means only `WARN` logs. 2 means also `INFO` logs. 3 means also `DEBUG` logs and 4 means also `TRACE` logs.

## Adding headers

The locations of header files are automatically detected. Even cmake automatically finds them. Any directory below the src
directory with the name `include` will form a global namespace of headers you can include with `#include<the_header.h>`.
That means that in contrast to what many c programs do (put all headers in one large folder), they are localized. For example,
the folder `src/vm/include` has all headers associated to virtual memory.  


## Running tests

Just `make test` (aka compile with the definition `ENABLE_TESTS=1`) and then running the kernel with semihosting. 


## Debugging

To debug the kernel in clion, one can use the provided run configuration (`debug`). The kernel will now wait on 
boot until the second run configuration (`attach debugger`) is ran. Breakpoints will work as normal.

A similar configuration can be achieved by running `make debug` in the `kernel/` directory (not in the project root) and 
then in a second terminal/tmux window/whatever run `make start_debug` to attach gdb with the right arguments.

## Running in clion

It's important to only ever run course_os with the provided makefile and ***never*** use cmake's run configuration. 
It doesn't work and can't ever work as the emulator must be started. 

# Current State of the kernel

Please read this before starting development on the kernel. This section includes the currently working (and not so working) parts in the kernel.
It's important to keep this section up-to-date so other groups can easily continue working, without backtracking every implementation.
Any points about the current implementation that might be improved upon, or other notes worth mentioning will be writtin in markdown quotes.

##  Virtual memory management
The kernel supports virtual memory management. The implementation, and header files can be found in the `vmm` folder.

## Loading of compiled ELF binaries
A loader is available in the `loader` folder. This can be used in conjuction with the `dispatcher` to start processes, and load them into memory.
At the time of writing there is no filesystem available, meaning that binaries have to be compiled _before_ compiling the kernel and injecting them into the compiled kernel using the CMake linker.

## Dispatcher
Switching between processes is done in the dispatcher. The dispatcher currently supports switching from User to SVC mode when calling software interrupts (SWI), as well as timer-based interrupts from any mode to IRQ. The interrupts are handled by the `interrupt_handler.s` assembly file, which can be found in `common`. 
The context switching is done by subroutines and macros in `scheduler/dispatcher.s` and `common/interrupt_handler.s`.

> The dispatcher can be written more cleanly by replacing some subroutines with macros. Also it would make more sense if the software interrupts (swi) and hardware interrupts (irq) code is in the same directory (currently they are split between `common/` and `scheduler/`.
> Interrupts should always be disabled during task switching.
> The current implementation for switching tasks when an IRQ occurs, is not very robust.

## Basic task scheduling
The scheduler can be fuond in the `scheduler` folder (duh). Currently, the scheduler is a simple circularly linked list where new processes are inserted at the end and popped from the front.
Scheduling new tasks always 
It also supports timeslicing, which is done by setting a set interval timer at initialization.

> There should be more information about a process than is currently the case. 
> There is not support for task prioritization


