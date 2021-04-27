#!/bin/sh
cd $MESON_SOURCE_ROOT/kernel
meson --cross-file $MESON_SOURCE_ROOT/cross_gcc.build build
cd build
ninja test-run
