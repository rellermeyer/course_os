########################################################################
## WARNING: This cmake file does *NOT* work to compile the course OS  ##
## It is merely here to make clion detect all the source files        ##
## so it can do code inspection. Use the supplied makefile to compile ##
########################################################################

cmake_minimum_required(VERSION 3.6)

project(course_os)

file(GLOB_RECURSE kernel_sources ./kernel/src/*.c ./kernel/src/**/*.c)
file(GLOB_RECURSE kernel_include LIST_DIRECTORIES true ./kernel/src/**/include)

set(SOURCE_FILES ${kernel_sources})

add_executable(course_os ${SOURCE_FILES})

message(${kernel_include})

include_directories(
    ${kernel_include}
)

