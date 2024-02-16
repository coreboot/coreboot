#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-only

#
# Imports ec_commands.h and ec_cmd_api.h from the cros EC repo
# and updates the copyright header for coreboot.
#

set -u

scratch_file=''
coreboot_top=''
pname=''
SPDX_ID_STRING="SPDX-License-Identifier"

cleanup()
{
	if [[ -n "${scratch_file}" ]]; then
		rm -f -- "${scratch_file}"
		scratch_file=''
	fi
}

trap cleanup EXIT

usage()
{
	cat <<- __EOF
	Usage: ${pname}: [top of cros EC repo]

	Imports ec_commands.h and ec_cmd_api.h from the cros EC repo
	and updates the copyright header for coreboot.
	__EOF
}

#
# Remove the original ChromiumOS copyright so we can insert the SPDX
# license identifier. Preserve the original number of lines in the file
# so embedded #line directives are correct.
#

update_copyright()
{
	local spdx="/* ${SPDX_ID_STRING}: BSD-3-Clause */"
	local f=$1

	# replace existing copyright with empty lines
	sed -i -e '/Copyright.*Chromium/,/^$/{s/^.*$//}' "${f}"
	# now add the SPDX header
	sed -i -e "1s@^\$@${spdx}@" "${f}"
}

get_file()
{
	local ec_path="$1"
	local ec_file="$2"
	local dst_path="$3"
	local log

	if [[ ! -f "${ec_path}/${ec_file}" ]]; then
		echo "${ec_path}/${ec_file} does not exist" 2>&1
		return 1
	fi

	scratch_file=$(mktemp)

	cp "${ec_path}/${ec_file}" "${scratch_file}"

	update_copyright "${scratch_file}"
	cp "${scratch_file}" "${dst_path}"

	rm -f "${scratch_file}"
	scratch_file=''

	log=$(git -C "${ec_path}" log --oneline -1 "${ec_file}")
	echo "The original ${ec_file} version in the EC repo is:"
	echo "  ${log}"
}

main()
{
	local dst_path
	local ec_path

	pname=$(basename "$0")

	if [[ $# != 1 ]]; then
		usage
		exit 1
	fi

	ec_path="$1"
	if [[ ! -d "${ec_path}" ]]; then
		echo "${pname}: could not find ${ec_path}" 1>&2
		exit 1
	fi

	coreboot_top=$(git rev-parse --show-toplevel)
	if [[ ! -d "${coreboot_top}" ]]; then
		echo "${pname}: could not determine coreboot top" 1>&2
		exit 1
	fi

	dst_path="${coreboot_top}/src/ec/google/chromeec"

	cat <<- __EOF
	Suggested commit message:

	Generated using ${pname} [EC-DIR].

	__EOF

	get_file "${ec_path}" include/ec_commands.h "${dst_path}/ec_commands.h"
	get_file "${ec_path}" include/ec_cmd_api.h "${dst_path}/ec_cmd_api.h"
	echo
}

main "$@"
