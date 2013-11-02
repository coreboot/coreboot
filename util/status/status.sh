#!/bin/sh
#
# This file is part of the coreboot project.
#
# Copyright (C) 2013 Google Inc.
# 

EXIT_SUCCESS=0
EXIT_FAILURE=1
OUTDIR="status"

# Stuff from command-line switches
REMOTE_HOST=""
CLOBBER_OUTPUT=0
UPLOAD_RESULTS=0

show_help() {
	echo "Usage:
	${0} <option>

Options
    -h
        Show this message.
    -c
        Clobber output when finished.
    -r  <host>
        Obtain machine information from remote host (using ssh).
    -u
        Upload results to coreboot.org.
"
}

# test a command
#
# $1: test command on remote host (0=no, 1=yes)
# $2: command to test
test_cmd()
{
	local rc

	if [ -e "$2" ]; then
		return
	fi

	if [[ $1 -eq 1 && "$REMOTE_HOST" ]]; then
		ssh root@${REMOTE_HOST} which "$2" >/dev/null
		rc=$?
	else
		which "$2" >/dev/null
		rc=$?
	fi

	if [ $rc -eq 0 ]; then
		return
	fi

	echo "$2 not found"
	exit $EXIT_FAILURE
}

# run a command
#
# $1: 0 to run command locally, 1 to run remotely if remote host defined
# $2: command
cmd()
{
	if [ -e "$2" ]; then
		return
	fi

	if [[ $1 -eq 1 && -n "$REMOTE_HOST" ]]; then
		ssh root@${REMOTE_HOST} "$2"
	else
		$2
	fi

	if [ $? -eq 0 ]; then
		return
	fi
		
	echo "Failed to run command: $2"
	exit $EXIT_FAILURE
}

while getopts "chr:u" opt; do
	case "$opt" in
		h)
			show_help
			exit $EXIT_SUCCESS
			;;
		c)
			CLOBBER_OUTPUT=1
			;;
		r)
			REMOTE_HOST="$OPTARG"
			;;
		u)
			UPLOAD_RESULTS=1
			;;
	esac
done

if [ -e "$OUTDIR" ]; then
	echo "Output directory exists, aborting."
	exit $EXIT_FAILURE
fi

mkdir "$OUTDIR"

getrevision="util/status/getrevision.sh"
test_cmd 0 $getrevision
touch ${OUTDIR}/revision.txt
printf "Local revision: %s\n" $($getrevision -l) >> ${OUTDIR}/revision.txt
printf "Upstream revision: %s\n" $($getrevision -u) >> ${OUTDIR}/revision.txt
printf "Upstream URL: %s\n" $($getrevision -U)>> ${OUTDIR}/revision.txt
printf "Timestamp: %s\n" $($getrevision -t) >> ${OUTDIR}/revision.txt

cbfstool_cmd="util/cbfstool/cbfstool"
test_cmd 0 "$cbfstool_cmd"
$cbfstool_cmd build/coreboot.rom extract -n config -f ${OUTDIR}/config.txt

test_cmd 1 "cbmem"
cmd 1 "cbmem -c" > ${OUTDIR}/coreboot_console.txt
cmd 1 "cbmem -t" > ${OUTDIR}/coreboot_timestamps.txt
cmd 1 "cbmem -C" > ${OUTDIR}/coreboot_coverage.txt

cmd 1 dmesg > ${OUTDIR}/kernel_log.txt

#if [ $UPLOAD_RESULTS -eq 1 ]; then
#	FIXME: implement this part
#fi

if [ $CLOBBER_OUTPUT -eq 1 ]; then
	rm -rf ${OUTDIR}
fi

exit $EXIT_SUCCESS
