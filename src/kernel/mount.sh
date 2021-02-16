#!/bin/bash

MOUNTPOINT=/media/05F0-8D41

if [ -e kernel.img ]; then
	rm ${MOUNTPOINT}/kernel.img
fi
make clean
make make_Pi
cp kernelPi.img kernel.img
cp kernel.img ${MOUNTPOINT}




