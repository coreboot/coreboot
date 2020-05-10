#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-only

set -x

IMAGE=$1

if [ ! -r "$IMAGE" ]; then
  echo "Can't find image $IMAGE."
  exit 1
fi

CBFSTOOL=$(which cbfstool)
if [ $? != 0 ]; then
  echo "Can't find cbfstool."
  exit 1
fi

IFDTOOL=$(which ifdtool)
if [ $? != 0 ]; then
  echo "Can't find ifdtool."
  exit 1
fi

$CBFSTOOL $IMAGE print

if [ $? -ne 0 ]; then
  echo "Not a coreboot image: $IMAGE"
  exit 1
fi

PCI=$($CBFSTOOL $IMAGE print|grep pci|cut -f1 -d\ )
MRC=$($CBFSTOOL $IMAGE print|grep mrc.bin|cut -f1 -d\ )

$CBFSTOOL $IMAGE extract -n $PCI -f $PCI
$CBFSTOOL $IMAGE extract -n $MRC -f $MRC
$IFDTOOL -x $IMAGE
mv flashregion_0_flashdescriptor.bin flashdescriptor.bin
mv flashregion_2_intel_me.bin me.bin
rm flashregion_*.bin
