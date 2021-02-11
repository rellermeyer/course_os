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
