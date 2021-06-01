#!/bin/sh
cd $MESON_BUILD_ROOT
arm-none-eabi-objcopy -I binary -O elf32-littlearm $1 dtb_binary.o
