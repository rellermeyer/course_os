#!/bin/bash

echo
echo "Write 'mk-process-wrapper.sh --help' to display the help."
echo

OUPUT_FILE='process_wrapper'

if [ "$1" == "--help" ]; then
    echo
    echo '- Use: mk-process-wrapper.sh <input-file> [<section-name>] [-s] '
    echo
    echo '-- Takes input-file as a C file or an Assembly file, compiles/assembles and links it'
    echo '-- into an executable ELF file, then it creates another'
    echo "-- object file $OUPUT_FILE.mo and adds a section inside"
    echo "-- of it named <section-name> containing the compiled ELF executable."
    echo
    echo "-- If you skip <section-name>, then a default is used for the section."
    echo "-- If you specify -s, then the input file is assumed to be an Assembly file"
    echo "-- instead of a C file and is assembled, not compiled."
    echo
    exit 0
fi

encaps_program="void encapsulate() {}"
echo "$encaps_program" > "$OUPUT_FILE.c"
arm-none-eabi-gcc -c -o "$OUPUT_FILE.mo" "$OUPUT_FILE.c" # Compile and link
rm -f "$OUPUT_FILE.c"

secname=$2
filename=$1


if [ -z "$filename" ]; then
    echo File name not provided!
    echo
    exit 1
fi

if [ -z "$secname" ] || [ "$secname" == "-s" ]; then
    echo Section name not provided! Using default.
    secname='exec_file'
fi


echo Creating ELF file "$OUPUT_FILE.mo" with file "$filename" in section "$secname" ...

if [ "$3" == "-s" ] || [ "$2" == "-s" ]; then
    echo Assembling ...
    arm-none-eabi-as -c -o "$filename.o" "$filename" # Assemble
else
  echo Compiling ...
  arm-none-eabi-gcc -c -o "$filename.o" "$filename" # Compile
fi

echo Linking ...
arm-none-eabi-ld  -static -e main -o "$filename.mo" "$filename.o" # Link

echo Creating section with executable file ...
arm-none-eabi-objcopy --add-section ".$secname=$filename.mo" "$OUPUT_FILE.mo" # Add section to output file

rm -f "$filename.o"
rm -f "$filename.mo"
