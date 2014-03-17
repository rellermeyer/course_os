#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
    printf "bootm 0x%X\n" $(expr $(stat -f%z ../u-boot/u-boot-$1/u-boot.bin) + 65536)
else
    printf "bootm 0x%X\n" $(expr $(stat -c%s ../u-boot/u-boot-$1/u-boot.bin) + 65536)
fi

