FROM fedora:33
MAINTAINER  Jan S. Rellermeyer <J.S.Rellermeyer@tudelft.nl>

RUN yum install -y arm-none-eabi-gcc-cs arm-none-eabi-binutils-cs arm-none-eabi-newlib qemu-system-arm-core make which findutils

WORKDIR /work
COPY src .
EXPOSE 1234

# Since we installed the toolchain from the repositories we don't need
# to install them separately. So we can just fake 'em.
RUN mkdir -p toolchain && touch toolchain/indicator
RUN make build
VOLUME build /work/kernel/build
