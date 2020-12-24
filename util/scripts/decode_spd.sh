#!/usr/bin/env bash
#
#
# SPDX-License-Identifier: GPL-2.0-only

# Parses spd hex files and outputs the contents in various formats
#
#
# Outputs csv, set, and json in same folder as SPD_HEX_FILE
#
# Example:
# 	decode_spd.sh ../../src/mainboard/google/zork/spd/micron-MT40A512M16TB-062E-J.spd.hex
#
# Outputs ../../src/mainboard/google/zork/spd/micron-MT40A512M16TB-062E-J.spd.{json|csv|set}
#
# TODO: This script assumes bincfg binary is at ../bincfg/bincfg (which is the
#	result of running the bincfg make), and the specs are at
#	../bincfg/*.spec. This dependency should be made more resilliant and
#	configurable.

set -e

function read8 () {
	echo $(( 16#$(xxd -s "${2}" -l 1 -p "${1}") ))
}

for file in "$@"
do
	bintmp=$(mktemp)
	outfile="${file%.hex}.set"

	echo "Decoding ${file}, outputting to ${outfile}"

	grep -v '^#' "${file}" | xxd -r -p - "${bintmp}"
	dram_type=$(read8 "${bintmp}" 2)
	if [ ! "${dram_type}" -eq 12 ]
	then
		#TODO: Handle other dram types
		printf "Error: Expecting dram4 (12), got %d\n" "${dram_type}"
		continue
	fi

	revision=$(read8 "${bintmp}" 1)
	if [ ! "${revision}" -eq $((0x13)) ]
	then
		printf "Warning: Expecting revision 0x13, got 0x%x.\n" "${revision}"
	fi

	module_type=$(read8 "${bintmp}" 3)
	case "${module_type}" in
	1) # RDIMM
		spec="../bincfg/ddr4_registered_spd_512.spec"
	;;
	2 | 3) #UDIMM | SO-DIMM
		spec="../bincfg/ddr4_unbuffered_spd_512.spec"
	;;
	* )
		printf "Error: Unhandled module type %d.\n" "${module_type}"
	;;
	esac

	../bincfg/bincfg -d "${spec}" "${bintmp}" "${outfile}"
	grep -v '^#' "${outfile}" | sed -e 's/ = \([^,]\+\)/: "\1"/g' \
		> "${file%.hex}.json"
	grep -v -e '^#' -e '^{' -e '^}' "${outfile}" | sed -e 's/=/,/g' \
		> "${file%.hex}.csv"
done
