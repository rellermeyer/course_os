project('kernel', 'c')

# variables to define in the preprocessor.
# amount of memory the PMM should try to allocate, may or may not match the actual amount of
# memory in the system. Stuff will crash if this number is more than the actual amount of
# memory. TODO: autodetect this at boot
add_project_arguments('-DMEM_DEBUG', language: 'c')
add_project_arguments('-DLOG_LEVEL=4', language: 'c')

c_args = []
link_args = []
includes = []

qemu_command = 'qemu-system-arm -m 1G -serial stdio -monitor none -M raspi2 -nographic -append "-load 0x41000 0x14000" -semihosting -cpu cortex-a7 -kernel '

gdb_command = 'arm-none-eabi-gdb -ex "target remote localhost:1234" -ex symbol-file "$MESON_BUILD_ROOT/kernel.elf"'


subdir('include')
subdir('src')

prog_python = import('python').find_installation('python3')

test_files = run_command(prog_python, 'src/test_suite/generate_tests.py')\
                        .stdout().strip().split('\n')
kernel_test = executable('kernel-test', sources: [kernel_sources, test_files],
                         name_suffix: 'elf', link_args: [link_args],
                         c_args: [c_args, '-DENABLE_TESTS'],
                         include_directories: includes)

kernel_elf = executable('kernel', kernel_sources, c_args : [c_args],
                        link_args: [link_args], name_suffix: 'elf',
                        include_directories: includes)

kernel_run = run_target('kernel-start', depends: [kernel_elf],
                        command: ['sh', '-c', qemu_command + '$MESON_BUILD_ROOT/kernel.elf'])

test_run = run_target('test-run', command: ['sh', '-c',
                                            '$MESON_SOURCE_ROOT/src/test_suite/run-tests.sh'])

kernel_debug = run_target('debug',
                          depends: [kernel_elf],
                          command: ['sh', '-c',
                                    qemu_command + '$MESON_BUILD_ROOT/kernel.elf ' \
                                    + '-S ' + '-s'])

kernel_gdb = run_target('gdb', depends: [kernel_elf], command: ['sh', '-c', gdb_command])
