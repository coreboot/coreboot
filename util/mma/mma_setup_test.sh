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

file_type_mma="mma"
flashrom_temp_image=/tmp/flash_image_mma.bin
mma_test_metadata_bin=mma_test_metadata.bin
mma_test_metadata_bin_path=/tmp/"${mma_test_metadata_bin}"

printf_separator() {
	printf '=%.0s' {1..80}
	printf '\n'
}

show_usage() {
	printf "usage: $(basename "${0}") %s\n" \
		"set [<mma_test_name_efi> <mma_test_param_bin>] | reset"
	printf '=%.0s' {1..29}
	printf "All possible MMA tests"
	printf '=%.0s' {1..29}
	printf "\n"
	printf_separator
	printf "Test Name \tMargin1D.efi\n"
        printf "Test Config\n"
        printf "\t\t%s %s\n" \
		Margin1DCkAllConfig.bin \
		Margin1DCmdAllConfig.bin \
		Margin1DCmdVrefConfig.bin \
		Margin1DCtlAllConfig.bin \
		Margin1DRxDqsDelayConfig.bin \
		Margin1DRxVrefConfig.bin \
		Margin1DTxDqDelayConfig.bin \
		Margin1DTxVrefConfig.bin
	printf_separator
	printf "Test Name \tMargin2D.efi\n"
	printf "Test Config\n"
	printf "\t\t%s %s\n" \
		Margin2D_Cmd_Ch0_D0_R0_Config.bin \
		Margin2D_Cmd_Ch0_D1_R0_Config.bin \
		Margin2D_Cmd_Ch1_D0_R0_Config.bin \
		Margin2D_Cmd_Ch1_D1_R0_Config.bin \
		Margin2D_Rx_Ch0_D0_R0_Config.bin \
		Margin2D_Rx_Ch0_D0_R1_Config.bin \
		Margin2D_Rx_Ch0_D1_R0_Config.bin \
		Margin2D_Rx_Ch0_D1_R1_Config.bin \
		Margin2D_Rx_Ch1_D0_R0_Config.bin \
		Margin2D_Rx_Ch1_D0_R1_Config.bin \
		Margin2D_Rx_Ch1_D1_R0_Config.bin \
		Margin2D_Rx_Ch1_D1_R1_Config.bin \
		Margin2D_Tx_Ch0_D0_R0_Config.bin \
		Margin2D_Tx_Ch0_D0_R1_Config.bin \
		Margin2D_Tx_Ch0_D1_R0_Config.bin \
		Margin2D_Tx_Ch0_D1_R1_Config.bin \
		Margin2D_Tx_Ch1_D0_R0_Config.bin \
		Margin2D_Tx_Ch1_D0_R1_Config.bin \
		Margin2D_Tx_Ch1_D1_R0_Config.bin \
		Margin2D_Tx_Ch1_D1_R1_Config.bin
	printf_separator
	printf "Test name \tMarginMapper.efi\n"
	printf "Test Config\n"
        printf "\t\t%s %s\n" \
		MarginMapperRxVref-RxDqsDelayConfigCh0.bin \
		MarginMapperRxVref-RxDqsDelayConfigCh1.bin \
		MarginMapperTxVref-TxDqDelayConfigCh0.bin \
		MarginMapperTxVref-TxDqDelayConfigCh1.bin \
		ScoreRxVref-RxDqsDelayConfigCh0.bin \
		ScoreRxVref-RxDqsDelayConfigCh1.bin \
		ScoreTxVref-TxDqDelayConfigCh0.bin \
		ScoreTxVref-TxDqDelayConfigCh1.bin
	printf_separator
	printf "Test Name \tRMT.efi\n"
	printf "Test Config\n"
        printf "\t\t%s %s\n" \
		RMTConfig.bin
	printf_separator
}

write_flash() {
	printf "Writing back flash contents "${flashrom_temp_image}"\n"
	flashrom -p host -w "${flashrom_temp_image}" -i BOOT_STUB --fast-verify || \
		{
			printf "failed to read flash\n" ;
			exit -1;
		}
}

remove_file_if_exists() {
	if [ -f "${1}" ]; then
		printf "removing old "${1}"\n"
		rm -f "${1}"
	fi
}

remove_metadata() {
	printf "Removing "${mma_test_metadata_bin}" from "${flashrom_temp_image}"\n"
	cbfstool "${flashrom_temp_image}" remove -n ${mma_test_metadata_bin}
	outout=$(cbfstool "${flashrom_temp_image}" print | \
			grep ${mma_test_metadata_bin})

	if [ -z "${outout}" ];then
		printf "Removed ${mma_test_metadata_bin} from %s\n" \
			"${flashrom_temp_image}"
	else
		printf "Failed to remove ${mma_test_metadata_bin} from %s\n" \
			"${flashrom_temp_image}"
		print_failed
		exit -1;
	fi
}

generate_metadata() {
	remove_file_if_exists "${mma_test_metadata_bin_path}"

	printf "Creating "${mma_test_metadata_bin_path}"\n"
	#
	#Format of $mma_test_metadata_bin
	#MMA_TEST_NAME=xxxxxx.efi;MMA_TEST_PARAM=xxxxxx.bin;
	#
	printf "MMA_TEST_NAME=${mma_test_name};MMA_TEST_PARAM=${mma_test_param};" \
		 > "${mma_test_metadata_bin_path}"
}

add_metadata_to_flashfile() {
	cbfstool "${flashrom_temp_image}" add -f "${mma_test_metadata_bin_path}" \
		-n ${mma_test_metadata_bin} -t ${file_type_mma} || \
		{
			printf "failed to add "${mma_test_metadata_bin_path}"\n" ;
			exit -1;
		}
}

cleanup() {
	remove_file_if_exists "${flashrom_temp_image}"
	remove_file_if_exists "${mma_test_metadata_bin_path}"
}

print_success() {
	printf "============== SUCCESS ==============\n"
}

print_failed() {
	printf "============== FAILED ==============\n"
}

#
# main entry point
#

main() {
	case "${#}:${1}" in
		(3:set)
			mma_test_name=${2}
			mma_test_param=${3}
			;;
		(1:reset)
			;;
		(*)
			show_usage
			exit -1
	esac

	if [ "${1}"  != "reset" ];then
		printf "mma_test_name = ${mma_test_name}\n"
		printf "mma_test_param = ${mma_test_param}\n"
	fi

	remove_file_if_exists "${flashrom_temp_image}"

	printf "Reading flash contents to "${flashrom_temp_image}"\n"
	flashrom -p host -r "${flashrom_temp_image}" -i BOOT_STUB || \
		{
			printf "failed to read flash\n" ;
			exit -1;
		}

	outdata=$(cbfstool "${flashrom_temp_image}" print | \
			grep ${mma_test_metadata_bin})

	case "$1" in
		("set")
			[ "${outdata}" ] && remove_metadata
			generate_metadata
			add_metadata_to_flashfile
			write_flash
			;;
		("reset")
			if [ -z "${outdata}" ];then
				printf "${mma_test_metadata_bin} not found at all.\n"
			else
				remove_metadata
				write_flash
			fi
			;;
	esac
	cleanup
	print_success
}

main "$@"
