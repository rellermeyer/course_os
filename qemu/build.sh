#!/bin/bash

QEMU_VERSION=5.2.0

{
	if [ ! -e qemu-${QEMU_VERSION}.tar.bz2 ]; then

		wget http://wiki.qemu-project.org/download/qemu-${QEMU_VERSION}.tar.bz2
	fi

	if [ ! -d qemu-${QEMU_VERSION} ]; then
		tar xvf qemu-${QEMU_VERSION}.tar.bz2
	fi

	cd qemu-${QEMU_VERSION} || exit 1; ./configure --prefix="$(pwd)/../qemu" --target-list=arm-softmmu && make all install
}
