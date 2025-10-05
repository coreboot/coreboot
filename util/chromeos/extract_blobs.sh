#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-only

# Default values
DIR=""
PLATFORM=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
	case $1 in
		-o|--output)
			DIR="$2"
			shift 2
			;;
		-p|--platform)
			PLATFORM="$2"
			shift 2
			;;
		-h|--help)
			echo "Usage: $0 [OPTIONS] IMAGE_FILE"
			echo ""
			echo "Extract blobs from a coreboot image"
			echo ""
			echo "OPTIONS:"
			echo "  -o, --output DIR     Output directory (default: $IMAGE-blobs)"
			echo "  -p, --platform PLAT  Platform for ifdtool (see platforms below)"
			echo "  -h, --help          Show this help message"
			echo ""
			echo "AVAILABLE PLATFORMS:"
			echo "  adl     - Alder Lake"
			echo "  aplk    - Apollo Lake"
			echo "  cnl     - Cannon Lake"
			echo "  lbg     - Lewisburg PCH"
			echo "  dnv     - Denverton"
			echo "  ehl     - Elkhart Lake"
			echo "  glk     - Gemini Lake"
			echo "  icl     - Ice Lake"
			echo "  ifd2    - IFDv2 Platform"
			echo "  jsl     - Jasper Lake"
			echo "  mtl     - Meteor Lake"
			echo "  sklkbl  - Sky Lake/Kaby Lake"
			echo "  tgl     - Tiger Lake"
			echo "  wbg     - Wellsburg"
			echo ""
			echo "EXAMPLES:"
			echo "  $0 -o ./blobs -p mtl bios.bin"
			echo "  $0 -p tgl image.bin"
			echo "  $0 -o custom-dir image.bin"
			exit 0
			;;
		-*)
			echo "Error: Unknown option $1"
			echo "Use -h or --help for usage information"
			exit 1
			;;
		*)
			if [ -z "$IMAGE" ]; then
				IMAGE="$1"
			else
				echo "Error: Multiple image files specified"
				exit 1
			fi
			shift
			;;
	esac
done

# Check if image file is provided
if [ -z "$IMAGE" ]; then
	echo "Error: You must provide an image file"
	echo "Use -h or --help for usage information"
	exit 1
fi

if [ ! -f "$IMAGE" ]; then
	echo "Error: Image file '$IMAGE' does not exist"
	exit 1
fi

# Set default output directory if not specified
if [[ "$DIR" = "" ]]; then
	# create new dir '$IMAGE-blobs' (less file extension)
	DIR=$(basename "$IMAGE")
	DIR="${DIR%.*}-blobs"
fi
mkdir -p "$DIR"

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

# ensure valid coreboot image / get list of CBFS contents
# try using RW_MAIN_A region first as it may contain newer
# files / files not present in the COREBOOT region
if $CBFSTOOL "$IMAGE" print -r FW_MAIN_A >"$DIR"/cbfs.txt 2>/dev/null; then
	REGION="FW_MAIN_A"
elif $CBFSTOOL "$IMAGE" print -r COREBOOT >"$DIR"/cbfs.txt 2>/dev/null; then
	# use COREBOOT region
	REGION="COREBOOT"
elif $CBFSTOOL "$IMAGE" print -r BOOT_STUB >"$DIR"/cbfs.txt 2>/dev/null; then
	# use BOOT_STUB region
	REGION="BOOT_STUB"
else
	echo "Error reading CBFS: $IMAGE is not a valid coreboot image"
	exit 1
fi

echo ""
echo "Extracting blobs from region $REGION..."
echo ""

# extract flash regions
IFDTOOL_ARGS="-x $IMAGE"
if [ -n "$PLATFORM" ]; then
	IFDTOOL_ARGS="-p $PLATFORM $IFDTOOL_ARGS"
fi

if ! $IFDTOOL $IFDTOOL_ARGS >/dev/null; then
	echo "Error reading flash descriptor/extracting flash regions"
	exit 1
fi
# rename to normal convention; drop unused regions
mv flashregion_0_flashdescriptor.bin "$DIR"/flashdescriptor.bin
[ -f flashregion_2_intel_me.bin ] && mv flashregion_2_intel_me.bin "$DIR"/me.bin
rm flashregion_*.bin

# extract microcode
if grep -q "cpu_microcode_blob.bin" "$DIR"/cbfs.txt; then
	$CBFSTOOL "$IMAGE" extract -r $REGION -n cpu_microcode_blob.bin -f "$DIR"/cpu_microcode_blob.bin
else
	echo "Note: cpu_microcode_blob.bin not found in CBFS, skipping..."
fi

# extract VGA BIOS
VGA=$(grep pci $DIR/cbfs.txt | cut -f1 -d\ )
if [ "$VGA" != "" ]; then
	$CBFSTOOL "$IMAGE" extract -r $REGION -n "$VGA" -f "$DIR/vgabios.bin"
fi

# extract MRC.bin
MRC=$(grep mrc.bin $DIR/cbfs.txt | cut -f1 -d\ )
if [ "$MRC" != "" ]; then
	$CBFSTOOL "$IMAGE" extract -r $REGION -n "$MRC" -f "$DIR/$MRC"
fi

# extract refcode
REF=$(grep refcode $DIR/cbfs.txt | cut -f1 -d\ )
if [ "$REF" != "" ]; then
	$CBFSTOOL "$IMAGE" extract -r $REGION -n fallback/refcode -f "$DIR/refcode.elf" -m x86
fi

# extract FSP blobs
for FSP in $(grep fsp $DIR/cbfs.txt | cut -f1 -d\ ); do
	$CBFSTOOL "$IMAGE" extract -r $REGION -n $FSP -f "$DIR/$FSP"
done

# extract audio blobs
for AUD in $(grep -e "-2ch-" -e "-4ch-" $DIR/cbfs.txt | cut -f1 -d\ ); do
	$CBFSTOOL "$IMAGE" extract -r $REGION -n $AUD -f "$DIR"/"$AUD"
done

# extract VBTs
for VBT in $(grep vbt $DIR/cbfs.txt | cut -f1 -d\ ); do
	$CBFSTOOL "$IMAGE" extract -r $REGION -n "$VBT" -f "$DIR"/"$VBT"
done

# extract IFWI
IFWI=$(cbfstool "$IMAGE" layout -w | grep IFWI)
if [ "$IFWI" != "" ]; then
	$CBFSTOOL "$IMAGE" read -r IFWI -f "$DIR"/ifwi.bin
fi

# generate hashes
(
	cd "$DIR"
	: >hashes.txt
	for FILE in $(ls *.{bin,elf} 2>/dev/null); do
		sha256sum "$FILE" >>hashes.txt
	done
)

# a little housekeeping
rm "$DIR"/cbfs.txt

echo ""
echo "All done"
