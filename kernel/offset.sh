#!/bin/bash
printf "bootm 0x%X\n" $(expr $(stat -c%s ../u-boot/u-boot-2010.03/u-boot.bin) + 65536)
