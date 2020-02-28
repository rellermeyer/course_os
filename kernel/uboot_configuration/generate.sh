#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo "
setenv bootargs $*@
bootm 0x210000
" > "$DIR/uboot-commands.ubt"
