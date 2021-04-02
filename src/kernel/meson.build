project('kernel', 'c')

# variables to define in the preprocessor.
# amount of memory the PMM should try to allocate, may or may not match the actual amount of
# memory in the system. Stuff will crash if this number is more than the actual amount of
# memory. TODO: autodetect this at boot
add_global_arguments('-DMEM_DEBUG', language: 'c')
add_global_arguments('-DLOG_LEVEL=4', language: 'c')

# Available options:
# * Any custom definition you'd like to have enabled
# * MEM_DEBUG - Compiles in part of the code that will print debug information about memory management
# * LOG_LEVEL (number between 0 and 4)
# DEFINITIONS = MEM_DEBUG LOG_LEVEL=LOG_LEVEL
# Get the sources
# Seeds the random order in which tests are run.
TESTS_SEED = 12

c_args = []
link_args = []
includes = []

subdir('include')
subdir('src')

prog_python = import('python').find_installation('python3')

test_files = run_command('python', 'src/test_suite/generate_tests.py', '12')\
                        .stdout().strip().split('\n')
kernel_test = executable('kernel-test', sources: [kernel_sources, test_files],
                         name_suffix: 'elf', link_args: [link_args],
                         c_args: [c_args, '-DENABLE_TESTS'],
                         include_directories: includes)

kernel_elf = executable('kernel', kernel_sources, c_args : [c_args],
                        link_args: [link_args], name_suffix: 'elf',
                        include_directories: includes)
