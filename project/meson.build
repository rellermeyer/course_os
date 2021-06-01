project('course_os')

test = run_target('run-test', command: ['sh', '-c', '$MESON_SOURCE_ROOT/scripts/test.sh'])
build = run_target('run-build', command: ['sh', '-c', '$MESON_SOURCE_ROOT/scripts/build.sh'])
run = run_target('run-kernel', command: ['sh', '-c', '$MESON_SOURCE_ROOT/scripts/run.sh'])
clean = run_target('run-clean', command: ['sh', '-c', '$MESON_SOURCE_ROOT/scripts/clean.sh'])
debug = run_target('run-debug', command: ['sh', '-c', '$MESON_SOURCE_ROOT/scripts/debug.sh'])
gdb = run_target('run-gdb', command: ['sh', '-c', '$MESON_SOURCE_ROOT/scripts/gdb.sh'])
doxygen = run_target('run-doxygen', command: ['sh', '-c', '$MESON_SOURCE_ROOT/scripts/doxygen.sh'])
