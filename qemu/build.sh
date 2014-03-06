#!/bin/bash

QEMU_VERSION=1.6.2

qemu-system-arm --version || {

	if [ ! -e qemu-${QEMU_VERSION}.tar.bz2 ]; then 
		wget http://wiki.qemu-project.org/download/qemu-${QEMU_VERSION}.tar.bz2
	fi

	if [ ! -d qemu-${QEMU_VERSION} ]; then 
		tar xvf qemu-${QEMU_VERSION}.tar.bz2
	fi

	cd qemu-${QEMU_VERSION}; ./configure --prefix=`pwd`/../qemu && make all install
}
