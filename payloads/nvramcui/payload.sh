#!/bin/sh
#
# This is a trivial payload compile & find script for abuild
#
DIR=`dirname $0`
lpgcc -o $DIR/nvramcui.elf $DIR/nvramcui.c 2>&1 >/dev/null || exit 1
echo "$DIR/nvramcui.elf"
