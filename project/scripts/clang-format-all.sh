#!/bin/bash

shopt -s globstar

DIR=$(dirname "$0")/..

eval clang-format -style=file $DIR/kernel/**/*.{c,h} -i

