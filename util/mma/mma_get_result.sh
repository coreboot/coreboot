#!/bin/bash

#
# This file is part of the coreboot project.
#
# Copyright (C) 2015 Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc.
#

mma_results_op_bin="${1}"
mma_results_op_bin_tmp="${mma_results_op_bin}".tmp
mma_cbmem_id="0x4d4d4144"

show_usage() {
	printf "usage: $(basename "${0}") <output_results.bin>\n"
	printf "pass path of a bin file where you want to save results.\n"
}

#
# main entry point
#

main() {
	if [ ! "${mma_results_op_bin}" ];then
		show_usage
		exit -1
	fi

	printf "Reading cbmem ...\n"
	cbmem -r ${mma_cbmem_id} > "${mma_results_op_bin_tmp}" || \
		{
			printf "error in executing cbmem utility\n" ;
			exit -1;
		}

	#format of o/p is :
	# <mma_signature><mma_test_header+data>
	#
	# where,
	# <mma_signature> is 32bit length string "MMAD"
	#
	# <mma_test_header+data>  is the FULL HOB which coreboot
	#			  receives from FSP
	# <mma_test_header> is 22 bytes long at the start of the HOB.
	# 		    MMA data starts right after 26 bytes
	# 		    26 bytes = (4 bytes of "MMAD"
	#				+ 22 bytes of mma_test_header)
	#

	mma_signature=$(dd if="${mma_results_op_bin_tmp}" bs=1 count=4 )

	if [[ ${mma_signature} != "MMAD" ]];then
		printf "MMA signature mismatch" > "${mma_results_op_bin}"
		rm -r "${mma_results_op_bin_tmp}"
		cbmem -l >> "${mma_results_op_bin}"
		printf "MMA signature mismatch\n"
		exit -1
	fi

	dd if="${mma_results_op_bin_tmp}" of="${mma_results_op_bin}" bs=1 skip=26 || \
		{
			printf "error in generating "${mma_results_op_bin}"\n" ;
			exit -1;
		}
	rm -r "${mma_results_op_bin_tmp}"
	printf "MMA data saved to "${mma_results_op_bin}"\n"
}

main "$@"
