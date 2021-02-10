FROM archlinux
MAINTAINER  Jan S. Rellermeyer <J.S.Rellermeyer@tudelft.nl>

RUN pacman -Sy --noconfirm -q qemu-headless qemu-headless-arch-extra arm-none-eabi-binutils arm-none-eabi-gcc arm-none-eabi-newlib make which

WORKDIR /work
COPY src .
EXPOSE 1234

# Since we installed the toolchain from the repositories we don't need
# to install them separately. So we can just fake 'em.
RUN touch toolchain/indicator
RUN make build
VOLUME build /work/kernel/build
