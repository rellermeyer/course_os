FROM fedora:33
MAINTAINER  Jan S. Rellermeyer <J.S.Rellermeyer@tudelft.nl>

RUN yum install -y arm-none-eabi-gcc-cs arm-none-eabi-binutils-cs arm-none-eabi-newlib qemu-system-arm-core python3 python3-jinja2 meson gcc

WORKDIR /work
COPY project .
EXPOSE 1234

# Since we installed the toolchain from the repositories we don't need
# to install them separately. So we can just fake 'em.
RUN cd kernel && meson build --cross-file ../cross_gcc.build && cd build && meson compile
VOLUME build /work/kernel/build
