#!/bin/bash

PREFIX=$(pwd)/arm-none-eabi
TARGET=$(pwd)/target
URL=ftp://ftp.gnu.org/gnu

# toolchain
GCC_VERSION="${GCC_VERSION:-9.2.0}"
BINUTILS_VERSION="${BINUTILS_VERSION:-2.34}"
GDB_VERSION="${GDB_VERSION:-9.1}"
GDB_EXT_VERSION=${GDB_VERSION}

rm -rf "${TARGET}"

mkdir -p "${TARGET}/orig"
mkdir -p "${TARGET}/src"
mkdir -p "${TARGET}/build"
mkdir -p "${TARGET}/buildroot"

cd "${TARGET}/orig"

if [ ! -e gcc-${GCC_VERSION}.tar.gz ]; then
	wget ${URL}/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz || exit 1;
fi

if [ ! -e gdb-${GDB_EXT_VERSION}.tar.gz ]; then
	wget ${URL}/gdb/gdb-${GDB_EXT_VERSION}.tar.gz || exit 1;
fi

if [ ! -e binutils-${BINUTILS_VERSION}.tar.bz2 ]; then
	wget ${URL}/binutils/binutils-${BINUTILS_VERSION}.tar.bz2 || exit 1;
fi

cd "${TARGET}/src"


if [ ! -d "${TARGET}/src/gcc-${GCC_VERSION}" ]; then
   tar xvf ../orig/gcc-${GCC_VERSION}.tar.gz || exit 1;

fi

if [ ! -d "${TARGET}/src/gdb-${GDB_EXT_VERSION}" ]; then
	tar xvf ../orig/gdb-${GDB_EXT_VERSION}.tar.gz || exit 1;
fi

if [ ! -d "${TARGET}/src/binutils-${BINUTILS_VERSION}" ]; then
	tar xvf ../orig/binutils-${BINUTILS_VERSION}.tar.bz2 || exit 1;
fi


if [ ! -e "${PREFIX}/bin/arm-none-eabi-ld" ]; then
	mkdir -p "${TARGET}/build/binutils-${BINUTILS_VERSION}"
	sed -i -e 's/@colophon/@@colophon/g' \
          -e 's/doc@cygnus.com/doc@@cygnus.com/g' "${TARGET}/src/binutils-${BINUTILS_VERSION}/bfd/doc/bfd.texinfo"
	cd "${TARGET}/build/binutils-${BINUTILS_VERSION}"
	../../src/binutils-${BINUTILS_VERSION}/configure \
		--target=arm-none-eabi \
		--prefix="${PREFIX}" \
		--enable-interwork \
		--enable-multilib \
		--with-gnu-as \
		--with-gnu-ld \
		--disable-nls \
		--disable-werror || exit 1;
	make all && make install || exit 1;
fi
export PATH="$PATH:${PREFIX}/bin"

mkdir -p "${TARGET}/build/gcc-${GCC_VERSION}"
sed -i 's/BUILD_INFO=info/BUILD_INFO =/g' "${TARGET}/src/gcc-${GCC_VERSION}/gcc/configure"
cd "${TARGET}/src/gcc-${GCC_VERSION}"
./contrib/download_prerequisites
cd "${TARGET}/build/gcc-${GCC_VERSION}"
../../src/gcc-${GCC_VERSION}/configure \
	--target=arm-none-eabi \
	--prefix="${PREFIX}" \
	--enable-interwork \
	--enable-multilib \
	--disable-nls \
	--with-system-zlib \
	--enable-languages="c" \
	--without-docdir \
	--disable-libssp \
	|| exit 1;
make all-gcc && make install-gcc || exit 1;

cd "${TARGET}/build/gcc-${GCC_VERSION}"
make all install || exit 1;

mkdir -p "${TARGET}/build/gdb-${GDB_VERSION}"
cd "${TARGET}/build/gdb-${GDB_VERSION}"
../../src/gdb-${GDB_VERSION}/configure --target=arm-none-eabi --prefix="${PREFIX}" --disable-werror --enable-interwork --enable-multilib || exit 1;
make all && make install || exit 1;

rm -rf "${TARGET}"
