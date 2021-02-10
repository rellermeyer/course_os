FROM archlinux
MAINTAINER  Jan S. Rellermeyer <J.S.Rellermeyer@tudelft.nl>

RUN pacman -Sy --noconfirm -q qemu-headless qemu-headless-arch-extra arm-none-eabi-binutils arm-none-eabi-gcc arm-none-eabi-newlib make

WORKDIR /work
COPY src .

# Since we installed the toolchain from the repositories we don't need
# to install them separately. So we can just fake 'em.
RUN touch toolchain/indicator
RUN make TOOLCHAIN_PATH=/usr/bin CC=arm-none-eabi-gcc QEMU=qemu-system-arm build
VOLUME build /work/kernel/build
