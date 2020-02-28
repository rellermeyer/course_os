#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

function quit_q() {
    killall -9 qemu-system-arm; exit $1
}

tail -f -n +1 "$DIR/../../test.log" | while read -r line
do
  if [[ $line =~ "TESTS COMPLETE" ]]; then
    echo "$line"
    quit_q 0
  elif [[ $line =~ "MEMORY LEAK" ]]; then
    echo "FAILED (MEMORY: $line)"
    quit_q 1
  elif [[ $line =~ "FAILED" ]]; then
    echo "FAILED"
    quit_q 2
  elif [[ $line =~ "DATA ABORT HANDLER" ]]; then
    echo "FAILED (EXCEPTION: $line)"
    quit_q 3
  else
    echo ">> $line"
  fi
done
