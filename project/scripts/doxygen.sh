#!/bin/sh
cd $MESON_SOURCE_ROOT
doxygen Doxyfile
ls
mv docs $MESON_BUILD_ROOT
xdg-open $MESON_BUILD_ROOT/docs/index.html
