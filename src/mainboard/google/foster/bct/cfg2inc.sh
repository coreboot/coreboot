#!/bin/sh
#
#
#
# SPDX-License-Identifier: GPL-2.0-only

bct_cfg2inc() {
	local in_file="$1"
	local out_file="$2"
	echo "{ /* generated from ${in_file}; do not edit. */" >"${out_file}"
	# Note currently we can only handle DDR3 type memory, even in C
	# implementation.
	sed "/^#.*$/d; s/^SDRAM.0./  /; s/\r$//; s/;$/,/;" \
		"${in_file}" >> "${out_file}"
	echo "}," >>"${out_file}"
}

for file in $@; do
	echo "Generating $file => ${file%cfg}inc..."
	bct_cfg2inc "${file}" "${file%cfg}inc"
done
