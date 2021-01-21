TOOLCHAIN_DIR=toolchain

BARE_METAL_TUPLE=arm-none-eabi
BARE_METAL_TARGET:=$(BARE_METAL_TUPLE)

#QEMU=./qemu/qemu/bin/qemu-system-arm
QEMU=/usr/bin/qemu-system-arm

#CFLAGS = -mcpu=arm1136j-s
CFLAGS = -mcpu=arm1176jz-s

export GCC_VERSION=10.2.0
export BINUTILS_VERSION=2.34
export GDB_VERSION=9.1
