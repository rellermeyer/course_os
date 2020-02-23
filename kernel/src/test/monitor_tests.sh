#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

tail -f -n +1 "$DIR/../../test.log" | while read line
do
  if [[ $line == *"TESTS COMPLETE"* ]]; then
    echo "COMPLETE"
    exit 0
  elif [[ $line == *"MEMORY LEAK"* ]]; then
    echo "FAILED (MEMORY: $line)"
    exit 1
  elif [[ $line == *"FAILED"* ]]; then
    echo "FAILED"
    exit 2
  elif [[ $line == *"DATA ABORT HANDLER"* ]]; then
    echo "FAILED (EXCEPTION: $line)"
    exit 3
  else
    echo "$line"
  fi
done

