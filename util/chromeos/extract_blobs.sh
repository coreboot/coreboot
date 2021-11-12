#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-only

if [ ! -f "$1" ]; then
	echo "Error: You must provide a valid filename"
	exit 1
fi

IMAGE=$1
# create new dir '$IMAGE-blobs' (less file extension)
DIR=$(basename $IMAGE)
DIR="${DIR%.*}-blobs"
mkdir -p $DIR

if [ -f ./cbfstool ]; then
	CBFSTOOL="./cbfstool"
else
	CBFSTOOL=$(command -v cbfstool)
fi
if [[ "$CBFSTOOL" = "" ]]; then
	echo "Error: cbfstool must be in your path or exist locally"
	exit 1
fi

if [ -f ./ifdtool ]; then
	IFDTOOL="./ifdtool"
else
	IFDTOOL=$(which ifdtool)
fi
if [[ "$IFDTOOL" = "" ]]; then
	echo "Error: ifdtool must be in your path or exist locally"
	exit 1
fi

# ensure valid coreboot image / get list of main COREBOOT CBFS contents
REGION=""
if ! $CBFSTOOL $IMAGE print >$DIR/cbfs.txt 2>/dev/null; then
	# try using BOOT_STUB region
	if ! $CBFSTOOL $IMAGE print -r BOOT_STUB >$DIR/cbfs.txt; then
		echo "Error reading CBFS: $IMAGE is not a valid coreboot image"
		exit 1
	else
		REGION="-r BOOT_STUB"
	fi
fi

echo ""
echo "Extracting blobs..."
echo ""

# extract flash regions
if ! $IFDTOOL -x $IMAGE >/dev/null; then
	echo "Error reading flash descriptor/extracting flash regions"
	exit 1
fi
# rename to normal convention; drop unused regions
mv flashregion_0_flashdescriptor.bin $DIR/flashdescriptor.bin
[ -f flashregion_2_intel_me.bin ] && mv flashregion_2_intel_me.bin $DIR/me.bin
rm flashregion_*.bin

# extract microcode
$CBFSTOOL $IMAGE extract $REGION -n cpu_microcode_blob.bin -f $DIR/cpu_microcode_blob.bin

# extract VGA BIOS
VGA=$(grep pci $DIR/cbfs.txt | cut -f1 -d\ )
if [ "$VGA" != "" ]; then
	$CBFSTOOL $IMAGE extract $REGION -n $VGA -f $DIR/vgabios.bin
fi

# extract MRC.bin
MRC=$(grep mrc.bin $DIR/cbfs.txt | cut -f1 -d\ )
if [ "$MRC" != "" ]; then
	$CBFSTOOL $IMAGE extract $REGION -n "$MRC" -f "$DIR/$MRC"
fi

# extract refcode
REF=$(grep refcode $DIR/cbfs.txt | cut -f1 -d\ )
if [ "$REF" != "" ]; then
	$CBFSTOOL $IMAGE extract $REGION -n fallback/refcode -f "$DIR/refcode.elf" -m x86
fi

# extract FSP blobs
for FSP in $(grep fsp $DIR/cbfs.txt | cut -f1 -d\ ); do
	$CBFSTOOL $IMAGE extract $REGION -n $FSP -f $DIR/$FSP
done

# extract audio blobs
for AUD in $(grep -e "-2ch-" -e "-4ch-" $DIR/cbfs.txt | cut -f1 -d\ ); do
	$CBFSTOOL $IMAGE extract $REGION -n $AUD -f $DIR/$AUD
done

# extract VBTs
for VBT in $(grep vbt $DIR/cbfs.txt | cut -f1 -d\ ); do
	$CBFSTOOL $IMAGE extract $REGION -n $VBT -f $DIR/$VBT
done

# extract IFWI
IFWI=$(cbfstool $IMAGE layout -w | grep IFWI)
if [ "$IFWI" != "" ]; then
	$CBFSTOOL $IMAGE read -r IFWI -f $DIR/ifwi.bin
fi

# generate hashes
(
	cd $DIR
	: >hashes.txt
	for FILE in $(ls *.{bin,elf} 2>/dev/null); do
		sha256sum $FILE >>hashes.txt
	done
)

# a little housekeeping
rm $DIR/cbfs.txt

echo ""
echo "All done"
