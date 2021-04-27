# Kernel Subsystem
In this directory all the code lives to generate the Course OS kernel. 

To build it you must first generate a build directory in case none already exists.
```bash
meson --cross-file ../cross_gcc.build
```

From that on you can run the following commands:
```bash
cd build

ninja gen-dtb      # Generate the dtb file, do this before building the kernel
ninja kernel.elf   # Build the kernel
ninja kernel-start # Start the QEMU emulation
ninja test-run     # Run the tests
ninja debug        # Start QEMU in debugging mode
ninja gdb          # Start GDB with the right configuration to hook onto QEMU

```
