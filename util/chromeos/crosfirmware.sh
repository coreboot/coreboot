#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-only

# On some systems, `parted` and `debugfs` are located in /sbin.
export PATH="$PATH:/sbin"

exit_if_dependencies_are_missing() {
	local missing_deps=()
	local -A deps_map=(
		["uudecode"]="sharutils"
		["debugfs"]="e2fsprogs"
		["parted"]="parted"
		["curl"]="curl"
		["unzip"]="unzip"
		["7z"]="p7zip"
	)

	# Check all dependencies at once
	for cmd_name in "${!deps_map[@]}"; do
		if ! type "$cmd_name" >/dev/null 2>&1; then
			missing_deps+=("$cmd_name:${deps_map[$cmd_name]}")
		fi
	done

	# Exit if any dependencies are missing
	if [ ${#missing_deps[@]} -gt 0 ]; then
		printf 'The following required commands were not found:\n' >&2
		for dep_info in "${missing_deps[@]}"; do
			cmd_name="${dep_info%%:*}"
			deb_pkg_name="${dep_info##*:}"
			printf '  - `%s`\n' "$cmd_name" >&2
			printf '    On Debian-based systems, install with: `apt install %s`\n' "$deb_pkg_name" >&2
		done
		exit 1
	fi
}

get_inventory() {
	_conf=$1
	_url=https://dl.google.com/dl/edgedl/chromeos/recovery/recovery.conf

	echo "Downloading recovery image inventory..."

	curl -s "$_url" >$_conf
}

download_image() {
	_url=$1
	_file=$2

	echo "Downloading recovery image"
	curl "$_url" >"$_file.zip"
	echo "Decompressing recovery image"
	unzip -q "$_file.zip"
	rm "$_file.zip"
}

extract_partition() {
	NAME=$1
	FILE=$2
	ROOTFS=$3
	_bs=1024

	echo "Extracting ROOT-A partition"
	ROOTP=$(printf "unit\nB\nprint\nquit\n" |
		parted $FILE 2>/dev/null | grep $NAME)

	if [ "$ROOTP" == "" ]; then
		# Automatic extraction failed, likely due to parted detecting
		# overlapping partitions. Fall back to using fdisk and assume
		# ROOT-A is partition #3
		echo "(Extracting via parted failed; falling back to fdisk)"
		_ssize=$(printf "p q" | fdisk $FILE | grep "Sector size" |
			cut -f2 -d: | cut -f2 -d ' ')
		_start=$(printf "p q" | fdisk $FILE | grep "bin3" | tr -s ' ' |
			cut -f2 -d ' ')
		_nsec=$(printf "p q" | fdisk $FILE | grep "bin3" | tr -s ' ' |
			cut -f4 -d ' ')
		START=$(($_ssize * $_start))
		SIZE=$(($_ssize * $_nsec))
	else
		START=$(($(echo $ROOTP | cut -f2 -d\  | tr -d "B")))
		SIZE=$(($(echo $ROOTP | cut -f4 -d\  | tr -d "B")))
	fi

	dd if=$FILE of=$ROOTFS bs=$_bs skip=$(($START / $_bs)) \
		count=$(($SIZE / $_bs)) >/dev/null 2>&1
}

extract_shellball() {
	ROOTFS=$1
	SHELLBALL=$2

	echo "Extracting chromeos-firmwareupdate"
	printf "cd /usr/sbin\ndump chromeos-firmwareupdate $SHELLBALL\nquit" |
		debugfs $ROOTFS >/dev/null 2>&1
}

extract_coreboot() {
	_shellball=$1
	_unpacked=$(mktemp -d)

	echo "Extracting coreboot image"
	if ! sh $_shellball --unpack $_unpacked >/dev/null 2>&1; then
		sh $_shellball --sb_extract $_unpacked >/dev/null 2>&1
	fi

	if [ -d $_unpacked/models/ ]; then
		_version=$(cat $_unpacked/VERSION | grep -m 1 -e Model.*$_board -A5 |
			grep "BIOS (RW) version:" | cut -f2 -d: | tr -d \ )
		if [ "$_version" == "" ]; then
			_version=$(cat $_unpacked/VERSION | grep -m 1 -e Model.*$_board -A5 |
				grep "BIOS version:" | cut -f2 -d: | tr -d \ )
		fi
		if [ -f $_unpacked/models/$_board/setvars.sh ]; then
			_bios_image=$(grep "IMAGE_MAIN" $_unpacked/models/$_board/setvars.sh |
				cut -f2 -d'"')
		else
			# special case for REEF, others?
			_version=$(grep -m1 "host" "$_unpacked/manifest.json" | cut -f12 -d'"')
			_bios_image=$(grep -m1 "image" "$_unpacked/manifest.json" | cut -f4 -d'"')
		fi
	elif [ -f "$_unpacked/manifest.json" ]; then
		_version=$(grep -m1 -A4 "$BOARD\":" "$_unpacked/manifest.json" | grep -m1 "rw" |
				sed 's/.*\(rw.*\)/\1/' | sed 's/.*\("Google.*\)/\1/' | cut -f2 -d'"')
		_bios_image=$(grep -m1 -A10 "$BOARD\":" "$_unpacked/manifest.json" |
				grep -m1 "image" | sed 's/.*"image": //' | cut -f2 -d'"')
	else
		_version=$(cat $_unpacked/VERSION | grep BIOS\ version: |
			cut -f2 -d: | tr -d \ )
		_bios_image=bios.bin
	fi
	if cp $_unpacked/$_bios_image coreboot-$_version.bin; then
		echo "Extracted coreboot-$_version.bin"
	fi
	rm -rf "$_unpacked"
	rm $_shellball
}

do_one_board() {
	_board=$1
	_url=$2
	_file=$3

	download_image $_url $_file

	extract_partition ROOT-A $_file root-a.ext2
	extract_shellball root-a.ext2 chromeos-firmwareupdate-$_board
	rm $_file root-a.ext2

	extract_coreboot chromeos-firmwareupdate-$_board
}

#
# Main
#

BOARD=${1,,}

exit_if_dependencies_are_missing

if [ "$BOARD" == "all" ]; then
	CONF=$(mktemp)
	get_inventory $CONF

	grep ^name= $CONF | while read _line; do
		name=$(echo $_line | cut -f2 -d=)
		echo Processing board $name
		eval $(grep -v hwid= $CONF | grep -A11 "$_line" |
			grep '\(url=\|file=\)')
		BOARD=$(echo $url | cut -f3 -d_)
		do_one_board $BOARD $url $file
	done

	rm "$CONF"
elif [ "$BOARD" != "" ]; then
	CONF=$(mktemp)
	get_inventory $CONF

	echo Processing board $BOARD
	eval $(grep -i -w $BOARD -A8 $CONF | grep '\(url=\|file=\)')
	do_one_board $BOARD $url $file

	rm "$CONF"
else
	echo "Usage: $0 <boardname>"
	echo "       $0 all"
	echo
	exit 1
fi
