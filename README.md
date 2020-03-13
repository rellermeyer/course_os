![course_os test suite](https://github.com/rellermeyer/course_os/workflows/course_os%20test%20suite/badge.svg)

# Course OS

This is a 'toy' kernel created by students from multiple universities.
Currently it is mainly developed by students of the TU Delft.
It is targeted to run on the raspberry pi zero, b+ and 2.

# Building

[Build parameters](kernel/README.md)

## Toolchain
To build and run the project, you will need a cross compiler. Since the kernel is made to run on ARM, the compiler has to output ARM instructions.

To build the c toolchain for ARM, and `qemu-system-arm` version 4.20, run
```bash
make requirements
```
from the root project directory. We build `qemu-system-arm` from source as this gives us better support
for attaching debuggers. Any up to date version of `qemu-system-arm` should work and if you already have this installed
through your package manager, just running
```bash
make toolchain
```
should be sufficient.

## Running

After you built the toolchain, the kernel can be ran with:
```bash
make run
```
from the root project directory, or from the `kernel` directory.

## Running tests

To run the test suite for the kernel, execute:

```bash
make test
```
from the root project directory, or from the `kernel` directory.


## Debugging

To debug the kernel, you have to perform two steps. First you have to build and start the kernel with
```bash
make debug
```

from the `kernel` directory. This prepares qemu so it waits for a debugger to be attached.

now, if you have CLion or VSCode you can run the supplied run configuration called `debug` which attaches a debugger, loads the sourcemap and runs the kernel. Now you can create breakpoints from within your IDE.

If however you don't have either of those IDEs, or want to use gdb from a terminal, one can run the following command from the kernel directory:
```bash
make start_debug
```

# Creating tests

To create tests for the kernel, please read [this](kernel/src/test/README.md) file.

# Editors

## CLion

To open this project in CLion, you can simply create a new `c` project with the supplied `CMakeLists.txt` file.
This file can *not* be used to actually run the kernel but it does give clion the right instructions to make code completion etc. work.

## Vim + CCLS

To generate the required `compile_commands.json` so that CCLS correctly index the project. You have to do the following:

First, generate the file using cmake:
```bash
cmake -H. -BDebug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
```
Then, copy the file to the root folder:
```bash
cp Debug/compile_commands.json .
```

Afterwards you can remove the `Debug` folder with:
```bash
rm -rf Debug
```

Now Vim and CCLS will correctly know how to index the project.



# Copyright

Copyright (c) 2015, <OWNER>
 All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

