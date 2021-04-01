#!/bin/bash

encaps_program="void encapsulate() {}"
echo "$encaps_program" > "simple_program.c"
arm-none-eabi-gcc -c -o "simple_program.mo" "simple_program.c" # Compile and link
rm -f "simple_program.c"

secname=$2
filename=$1


if [ -z "$filename" ]; then
    echo File name not provided!
    exit 1
fi

if [ -z "$secname" ]; then
    echo Section name not provided!
    exit 1
fi


echo Creating ELF file with file "$filename" in section "$secname"...

arm-none-eabi-gcc -c -o "$filename.o" "$filename" # Compile and link
arm-none-eabi-ld  -static -e main -o "$filename.mo" "$filename.o"

rm -f "$filename.o"

arm-none-eabi-objcopy --add-section ".$secname=$filename.mo" "simple_program.mo"

rm -f "$filename.mo"
