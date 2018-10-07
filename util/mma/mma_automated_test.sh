#!/usr/bin/env bash

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
# MERCHANTABILITY or FIMMA_TEST_NAMEESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#

MMA_LOCAL_DATA_STORAGE=/usr/local/mma
#
# format of MMA_AUTOMATED_TEST_CONFIG file is as below
# with 1 or more repeated lines with same format
#
# <testname> <testparam> <#times to run this test>
#
# e.g.
# RMT.efi RMTConfig.bin 2
# Margin1D.efi Margin1DCmdAllConfig.bin 1
#
#
MMA_AUTOMATED_TEST_CONFIG="${MMA_LOCAL_DATA_STORAGE}"/tests
MMA_AUTOMATED_TEST_COUNT="${MMA_LOCAL_DATA_STORAGE}"/count
MMA_SETUP_TEST_TOOL=mma_setup_test.sh
MMA_GET_RESULT_TOOL=mma_get_result.sh
MMA_TEST_RESULTS_PATH="${MMA_LOCAL_DATA_STORAGE}/results$(date +_%y_%m_%d_%H_%M)"
# Clear MMA_TEST_NUMBER just in case it is set in environment
MMA_TEST_NUMBER=

# Set a number of global params based on test number
# MMA_TEST_NUMBER - test number, stored in MMA_AUTOMATED_TEST_COUNT
# MMA_TEST_NAME  - test name
# MMA_TEST_PARAM - test parameter
# MMA_TEST_COUNT - test count, number of times to run the test
# MMA_TEST_RESULT_NAME - filename for result
get_mma_autotest_params() {
	typeset -i i=${MMA_TEST_NUMBER}
	exec 9< "${MMA_AUTOMATED_TEST_CONFIG}"
	while read -u9 MMA_TEST_NAME MMA_TEST_PARAM MMA_TEST_COUNT
	do
		case "${MMA_TEST_NAME}" in
			("#"*|"") continue;; # Allow blank lines and comments
		esac
		: $(( i -= MMA_TEST_COUNT ))
		if (( i <= 0 )) ; then
			printf -v MMA_TEST_RESULT_NAME \
			"${MMA_TEST_NAME%.efi}_${MMA_TEST_PARAM%.bin}_%d.bin" \
				$((MMA_TEST_COUNT+i))
			return
		fi
	done
	${MMA_SETUP_TEST_TOOL} reset
	rm "${MMA_AUTOMATED_TEST_COUNT}"
	mv "${MMA_AUTOMATED_TEST_CONFIG}" "${MMA_TEST_RESULTS_PATH}"
	exit 0
}

main() {
	# sleep 30 sec, before we start. This would give some time if we want
	# to stop automation.
	sleep 30s
	mkdir -p "${MMA_LOCAL_DATA_STORAGE}"
	# Exit if there are no tests
	[ -e "${MMA_AUTOMATED_TEST_CONFIG}" ] || exit 0

	if [ -e "${MMA_AUTOMATED_TEST_COUNT}" ] ; then
		. "${MMA_AUTOMATED_TEST_COUNT}"
	fi

	mkdir -p "${MMA_TEST_RESULTS_PATH}"

	if [ "${MMA_TEST_NUMBER}" ] ; then
		get_mma_autotest_params
		${MMA_GET_RESULT_TOOL} \
			"${MMA_TEST_RESULTS_PATH}"/"${MMA_TEST_RESULT_NAME}"
	fi

	: $(( MMA_TEST_NUMBER += 1 ))
	printf "MMA_TEST_NUMBER=${MMA_TEST_NUMBER}\n" \
			> "${MMA_AUTOMATED_TEST_COUNT}"
	printf "MMA_TEST_RESULTS_PATH=%s" "${MMA_TEST_RESULTS_PATH}" \
			>> "${MMA_AUTOMATED_TEST_COUNT}"
	get_mma_autotest_params
	${MMA_SETUP_TEST_TOOL} set ${MMA_TEST_NAME} ${MMA_TEST_PARAM}

	# sync the filesystem, hoping this would minimize
	# the chances of fs corruption
	sync
	sleep 2s
	sync
	sleep 2s
	sync
	sleep 2s
	ectool reboot_ec
}

main "$@"
