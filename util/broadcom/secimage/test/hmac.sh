#!/usr/bin/env bash

##
## This file is part of the coreboot project.
##
## Copyright (C) 2003-2018 Alex Thiessen <alex.thiessen.de+coreboot@gmail.com>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 3 or later of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## SPDX-License-Identifier: GPL-3.0-or-later
## <https://spdx.org/licenses/GPL-3.0-or-later.html>
##

set -o errexit
set -o nounset
set -o pipefail

# static analysis
if command -v shellcheck 1>/dev/null; then
	shellcheck "${BASH_SOURCE[0]}"
else
       echo "shellcheck not found, running unchecked" >&2
fi

# dependency check
dependencies=(basename diff dirname head mkdir mktemp openssl rm tail xxd)
for dependency in "${dependencies[@]}"; do
	if ! command -v "${dependency}" 1>/dev/null; then
		echo "missing ${dependency}, test skipped" >&2
		exit 0
	fi
done

# parameters
if [ ${#} -ne 1 ]; then
	echo "usage: '${0}' <testee>"
	exit 1
fi

# setup
testee="${1}"
declare -i header_len=16 signature_len=32
tmp_dir="$(mktemp --directory --tmpdir secimage-test-XXXXXXXX)"
shopt -s globstar nullglob
for dump_file in test/data/**/*.xxdump; do
	bin_file_dir="${tmp_dir}/$(dirname "${dump_file#test/data/}")"
	mkdir --parents "${bin_file_dir}"
	xxd -r "${dump_file}" \
	       "${bin_file_dir}/$(basename "${dump_file}" .xxdump)"
done
tail --bytes=+$((header_len + 1)) "${tmp_dir}/expected/binary" \
	| head --bytes=-${signature_len} \
	| openssl dgst -sha256 -mac hmac \
		-macopt hexkey:"$(xxd -c$((signature_len * 2)) -ps \
			"${tmp_dir}/input/hmac_binary_key")" \
		-binary \
		> "${tmp_dir}/expected/signature"
mkdir "${tmp_dir}/actual"

# test
"${testee}" \
	-out "${tmp_dir}/actual/binary" \
	-config "${tmp_dir}/input/configfile" \
	-hmac "${tmp_dir}/input/hmac_binary_key" \
	-bl "${tmp_dir}/input/binary"
tail --bytes=${signature_len} "${tmp_dir}/actual/binary" \
	> "${tmp_dir}/actual/signature"
diff --recursive "${tmp_dir}/actual" "${tmp_dir}/expected" 1>/dev/null

# teardown
rm --force --recursive "${tmp_dir}"
