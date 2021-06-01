#!/bin/sh
$MESON_SOURCE_ROOT/src/test_suite/generate_tests.py -1 @> /dev/null
cd $MESON_BUILD_ROOT
ninja kernel-test.elf
qemu-system-arm -m 1G -serial stdio -monitor none -M raspi2 -nographic -append "-load 0x41000 0x14000" -semihosting -cpu cortex-a7 -kernel kernel-test.elf
