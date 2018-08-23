#!/bin/bash
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
# converts a depthcharge fmap.dts into an fmaptool compatible .fmd format
# requires fdt utilities (dtc, fdtget)
#
# $1 dts file name
# result on stdout
set -e

if [ $# -lt 1 ]; then
	echo "usage: $0 dts-file"
	exit 1
fi

DTS=$1
DTB=`mktemp`

# $1 node
# $2 start variable name
# $3 size variable name
get_reg() {
	local t=`fdtget -t x $DTB $1 reg 2>/dev/null`
	if [ -n "$t" ]; then
		export $2=0x`echo $t|cut -d' ' -f1`
		export $3=0x`echo $t|cut -d' ' -f2`
	else
		export $3=0x`fdtget -t x $DTB $1 size`
	fi
}

dtc -O dtb -o $DTB $DTS
get_reg /flash ROM_START ROM_SIZE
printf "FLASH@${ROM_START} ${ROM_SIZE} {"

PREFIX="\t"
REGION_START=-1
REGION_SIZE=0
CONTAINER_END=$(( ${ROM_SIZE} ))
CONTAINER_END_STACK=${CONTAINER_END}
CONTAINER_OFFSET=0
CONTAINER_OFFSET_STACK=0

FMAP_REGIONS=`fdtget -l $DTB /flash`
for region in $FMAP_REGIONS; do
	OLD_REGION_START=$REGION_START
	OLD_REGION_SIZE=$REGION_SIZE
	get_reg /flash/$region REGION_START REGION_SIZE

	# determine if we're past an existing container
	while [ $(( ${REGION_START} )) -ge ${CONTAINER_END} ]; do
		PREFIX=`printf "${PREFIX}" | cut -c2-`
		printf "\n${PREFIX}}"
		CONTAINER_END_STACK=`printf "${CONTAINER_END_STACK}" | cut -d' ' -f2-`
		CONTAINER_OFFSET_STACK=`printf "${CONTAINER_OFFSET_STACK}" | cut -d' ' -f2-`
		CONTAINER_END=`printf ${CONTAINER_END_STACK} | cut -d' ' -f1`
		CONTAINER_OFFSET=`printf ${CONTAINER_OFFSET_STACK} | cut -d' ' -f1`
	done

	# determine if we're inside a new container region now
	if [ $(( ${OLD_REGION_START} + ${OLD_REGION_SIZE} )) -gt $(( ${REGION_START} )) ]; then
		PREFIX="\t${PREFIX}"
		CONTAINER_END=$(( ${OLD_REGION_START} + ${OLD_REGION_SIZE} ))
		CONTAINER_OFFSET=$(( ${OLD_REGION_START} ))
		CONTAINER_END_STACK="${CONTAINER_END} ${CONTAINER_END_STACK}"
		CONTAINER_OFFSET_STACK="${CONTAINER_OFFSET} ${CONTAINER_OFFSET_STACK}"
		printf " {"
	fi

	LOCAL_REGION_START=$(( ${REGION_START} - ${CONTAINER_OFFSET} ))
	LOCAL_REGION_START=`printf "0x%x" ${LOCAL_REGION_START}`

	REGION_NAME=`fdtget $DTB /flash/$region label | tr '[a-z]-' '[A-Z]_'`
	REGION_TYPE=`fdtget $DTB /flash/$region type 2>/dev/null | cut -d'/' -f1`

	# a CBFS region? if so, mark as such
	if [ "${REGION_TYPE}" = "blob cbfs" ]; then
		IS_CBFS="(CBFS)"
	else
		IS_CBFS=""
	fi

	# special handling: rename BOOT_STUB to COREBOOT, mark them as CBFS
	if [ "${REGION_NAME}" = "BOOT_STUB" ]; then
		REGION_NAME="COREBOOT"
	fi
	if [ "${REGION_NAME}" = "COREBOOT" ]; then
		IS_CBFS="(CBFS)"
	fi

	# also mark RW_LEGACY (seabios et al) as CBFS
	if [ "${REGION_NAME}" = "RW_LEGACY" ]; then
		IS_CBFS="(CBFS)"
	fi

	# special handling: COREBOOT region at 0, inject a 128K bootblock
	# The size may need changes to accommodate the chipsets,
	# but should work for now.
	if [ "${REGION_NAME}" = "COREBOOT" -a \
		$(( ${REGION_START} )) -eq 0 ]; then
		printf "\n${PREFIX}BOOTBLOCK@0 128K"
		LOCAL_REGION_START=$(( ${LOCAL_REGION_START} + 128*1024 ))
		LOCAL_REGION_START=`printf 0x%x ${LOCAL_REGION_START}`
		REGION_SIZE=$(( ${REGION_SIZE} - 128*1024 ))
		REGION_SIZE=`printf 0x%x ${REGION_SIZE}`
	fi

	printf "\n${PREFIX}${REGION_NAME}${IS_CBFS}@${LOCAL_REGION_START} ${REGION_SIZE}"
done

while [ -n "${PREFIX}" ]; do
	PREFIX=`printf "${PREFIX}" | cut -c2-`
	printf "\n${PREFIX}}"
done
printf "\n"

rm -f $DTB
