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
| CPU | The cpu type emulated by qemu. Supported cpu types are the `arm1176` and `cortex-a7`|


## Definitions

| Definition | Action |
| --- | --- |
| ENABLE_TESTS | With this definition enabled, tests are compiled into the kernel |
| MEM_DEBUG | Every kernel heap memory allocation (kmalloc, kfree) will be logged at TRACE level. Tests will be checked for memory leaks. |
| LOG_LEVEL | Number between 1 and 4 describing the log level of the kernel. 1 means only `WARN` logs. 2 means also `INFO` logs. 3 means also `DEBUG` logs and 4 means also `TRACE` logs.  