#!/bin/sh
#
# This file is part of the coreboot project.
#
# Copyright 2015 Google Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

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
