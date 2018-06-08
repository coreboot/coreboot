#!/bin/sh
#
# This is a trivial payload compile & find script for abuild
#
DIR=$(dirname "$0")
lpgcc -o "$DIR/nvramcui.elf" "$DIR/nvramcui.c" >/dev/null 2>&1 || exit 1
echo "$DIR/nvramcui.elf"
