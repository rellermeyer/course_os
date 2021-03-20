########################################################################
## WARNING: This cmake file does *NOT* work to compile the course OS  ##
## It is merely here to make clion detect all the source files        ##
## so it can do code inspection. Use the supplied makefile to compile ##
########################################################################

cmake_minimum_required(VERSION 3.6)

project(course_os)

set(CMAKE_SYSTEM_NAME linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/src/toolchain/arm-none-eabi/bin/arm-none-eabi-gcc)
set(CMAKE_C_LINK_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/src/toolchain/arm-none-eabi/bin/arm-none-eabi-ld)

file(GLOB_RECURSE kernel_sources ./src/kernel/src/*.c ./src/kernel/src/**/*.c ./src/kernel/src/**/*.s)
file(GLOB_RECURSE kernel_include LIST_DIRECTORIES true ./src/kernel/src/**/include)

set(SOURCE_FILES ${kernel_sources})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_compile_definitions(ENABLE_TESTS MEM_DEBUG)

add_executable(course_os ${SOURCE_FILES})

message(${kernel_include})

include_directories(
    ${kernel_include}
)

