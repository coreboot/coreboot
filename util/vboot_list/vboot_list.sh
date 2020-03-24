#!/usr/bin/env bash

TOP="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/../.. >/dev/null 2>&1 && pwd )"
MAINBOARDS="src/mainboard"
OUTPUT_FILE=${1:-$TOP/Documentation/security/vboot/list_vboot.md}

function has_vboot
{
	local DIR=$1

	grep -rq "config VBOOT" $DIR
	return $?
}

function get_vendor_name
{
	local VENDORDIR=$1

	sed -n '/config VENDOR/{n;s/^[\t[:space:]]\+bool "\(.*\)"/\1/;p;}' \
		$VENDORDIR/Kconfig.name
}

function get_board_name
{
	local BOARDDIR=$1

	sed -n '/config BOARD/{n;s/^[\t[:space:]]\+bool "\(->\s\+\)\?\(.*\)"/\2/;p;}' \
		$BOARDDIR/Kconfig.name
}

function list_vboot_boards
{
	local VENDORDIR=$1
	for BOARD in $(ls -d $VENDORDIR/*/)
	do
		has_vboot $BOARD || continue
		get_board_name $BOARD
	done
}

function generate_vboot_list
{
for VENDOR in $(ls -d $TOP/$MAINBOARDS/*/)
do
	has_vboot $VENDOR || continue
	echo -e "\n## $(get_vendor_name $VENDOR)"
	IFS=$'\n'
	for BOARD in $(list_vboot_boards $VENDOR)
	do
		echo "- $BOARD"
	done
done
}

(echo "# vboot-enabled devices"; generate_vboot_list) > $OUTPUT_FILE
