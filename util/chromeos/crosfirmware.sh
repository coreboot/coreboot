#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-only

# On some systems, `parted` and `debugfs` are located in /sbin.
export PATH="$PATH:/sbin"

exit_if_uninstalled() {
	local cmd_name="$1"
	local deb_pkg_name="$2"

	if type "$cmd_name" >/dev/null 2>&1; then
		return
	fi

	printf '`%s` was not found. ' "$cmd_name" >&2
	printf 'On Debian-based systems, it can be installed\n' >&2
	printf 'by running `apt install %s`.\n' "$deb_pkg_name" >&2

	exit 1
}

exit_if_dependencies_are_missing() {
	exit_if_uninstalled "uudecode" "sharutils"
	exit_if_uninstalled "debugfs" "e2fsprogs"
	exit_if_uninstalled "parted" "parted"
	exit_if_uninstalled "curl" "curl"
	exit_if_uninstalled "unzip" "unzip"
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
		_version=$(grep -m1 -A1 "$BOARD\":" "$_unpacked/manifest.json" | grep "host" | cut -f12 -d'"')
		_bios_image=$(grep -m1 -A3 "$BOARD\":" "$_unpacked/manifest.json" | grep "image" | cut -f4 -d'"')
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
	eval $(grep -i $BOARD -A8 $CONF | grep '\(url=\|file=\)')
	do_one_board $BOARD $url $file

	rm "$CONF"
else
	echo "Usage: $0 <boardname>"
	echo "       $0 all"
	echo
	exit 1
fi
