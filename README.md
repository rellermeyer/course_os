![course_os test suite](https://github.com/rellermeyer/course_os/workflows/course_os%20test%20suite/badge.svg)

# Course OS

This is a 'toy' kernel created by students from multiple universities.
Currently it is mainly developed by students of the TU Delft.
It is targeted to run on the raspberry pi zero, b+ and 2.

# Building

[Build parameters](kernel/README.md)

## Docker
One option to run the project is to make use of the [Docker](https://www.docker.com/) image. To do so you first need to download Docker and Docker Compose for your operating system, after which you can run the follow commands to build and start the container:

```bash
docker-compose build
docker-compose up -d
```

After this you can choose to open the shell to the container by running:
```bash
docker-compose run raspberry
```
After that you can follow the instructions starting at [Running](#running)

You can find the directory with the built kernel at:`/var/lib/docker/course_os_kernel/_data`
To run the kernel you can use the command:
```base
qemu-system-arm -kernel kernel.elf -m 1G -serial stdio -monitor none -M raspi2 -cpu cortex-a7 -nographic -append "-load 0x410000 0x14000" -semihosting
```


## Toolchain
To build and run the project, you will need a cross compiler. Since the kernel is made to run on ARM, the compiler has to output ARM instructions.
You can find in scripts to install known working versions of qemu and the compilation toolchain. Run those and add the generated bin directories to your PATH.

## Running

After you built the toolchain go into the project directory and generate a build directory.
```bash
cd project
meson --cross-file cross_gcc.build build
```

Having generated the directory you can cd into the build directory and run the kernel with the following command.
```bash
ninja run-kernel
```

## Running tests

To run the test suite for the kernel, execute:

```bash
ninja run-test
```

## Debugging

To debug the kernel, you have to perform two steps. First you have to build and start the kernel with
```bash
ninja run-debug
```

This prepares qemu so it waits for a debugger to be attached.

now, if you have CLion or VSCode you can run the supplied run configuration called `debug` which attaches a debugger, loads the sourcemap and runs the kernel. Now you can create breakpoints from within your IDE.

If however you don't have either of those IDEs, or want to use gdb from a terminal, one can run the following command:
```bash
ninja run-gdb
```

# Creating tests

To create tests for the kernel, please read [this](kernel/src/test/README.md) file.

# Editors

## CLion

To open this project in CLion, you can simply create a new `c` project with the supplied `CMakeLists.txt` file.
This file can *not* be used to actually run the kernel but it does give clion the right instructions to make code completion etc. work.

## Emacs/Vim + CCLS

You can use the `compile_commands.json` from the kernel build directory by creating a softlink from that file to this directory. Doing so will keep it up to date.

## Emacs + GDB

A small lisp script to set the correct GDB command is included in the `.dir-locals.el` file. This will run automatically when you open any file in the project directory. You may receive a prompt related to this. After you start a debug session with `make debug`, you should be able to run the `gdb` command in Emacs and the correct command should already be in the prompt.
