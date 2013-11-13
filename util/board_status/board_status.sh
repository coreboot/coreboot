#!/bin/sh
#
# This file is part of the coreboot project.
#
# Copyright (C) 2013 Google Inc.
#

EXIT_SUCCESS=0
EXIT_FAILURE=1

# Stuff from command-line switches
REMOTE_HOST=""
CLOBBER_OUTPUT=0
UPLOAD_RESULTS=0

# Used to specify whether a command should always be run locally or
# if command should be run remoteley when a remote host is specified.
LOCAL=0
REMOTE=1

show_help() {
	echo "Usage:
	${0} <option>

Options
    -h
        Show this message.
    -C
        Clobber temporary output when finished. Useful for debugging.
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

	if [[ $1 -eq $REMOTE && -n "$REMOTE_HOST" ]]; then
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

	if [[ $1 -eq $REMOTE && -n "$REMOTE_HOST" ]]; then
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

while getopts "Chr:u" opt; do
	case "$opt" in
		h)
			show_help
			exit $EXIT_SUCCESS
			;;
		C)
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

grep -rH 'coreboot.org' .git/config >/dev/null 2>&1
if [ $? -ne 0 ]; then
	echo "Script must be run from root of coreboot directory"
	exit $EXIT_FAILURE
fi

# Results will be placed in a temporary location until we're ready to upload.
# If the user does not wish to upload, results will remain in /tmp.
tmpdir=$(mktemp -d)

# Obtain board and revision info to form the directory structure:
# <vendor>/<board>/<revision>/<timestamp>
cbfstool_cmd="util/cbfstool/cbfstool"
test_cmd $LOCAL "$cbfstool_cmd"
$cbfstool_cmd build/coreboot.rom extract -n config -f ${tmpdir}/config.txt
mainboard_dir="$(grep CONFIG_MAINBOARD_DIR ${tmpdir}/config.txt | awk -F '"' '{ print $2 }')"
vendor=$(echo "$mainboard_dir" | awk -F '/' '{ print $1 }')
mainboard=$(echo "$mainboard_dir" | awk -F '/' '{ print $2 }')

getrevision="util/board_status/getrevision.sh"
test_cmd $LOCAL $getrevision
tagged_version=$($getrevision -T)
timestamp=$($getrevision -t)

results="${vendor}/${mainboard}/${tagged_version}/${timestamp}"

echo "Temporarily placing output in ${tmpdir}/${results}"
mkdir -p "${tmpdir}/${results}"

mv "${tmpdir}/config.txt" "${tmpdir}/${results}"

touch ${tmpdir}/${results}/revision.txt
printf "Local revision: %s\n" "$($getrevision -l)" >> ${tmpdir}/${results}/revision.txt
printf "Tagged revision: %s\n" "${tagged_version}" >> ${tmpdir}/${results}/revision.txt
printf "Upstream revision: %s\n" $($getrevision -u) >> ${tmpdir}/${results}/revision.txt
printf "Upstream URL: %s\n" $($getrevision -U)>> ${tmpdir}/${results}/revision.txt
printf "Timestamp: %s\n" "$timestamp" >> ${tmpdir}/${results}/revision.txt

test_cmd $REMOTE "cbmem"
cmd $REMOTE "cbmem -c" > ${tmpdir}/${results}/coreboot_console.txt

# TODO: Some commands should be optional and be non-fatal in case of error.
#cmd $REMOTE "cbmem -t" > ${outdir}/coreboot_timestamps.txt

cmd $REMOTE dmesg > ${tmpdir}/${results}/kernel_log.txt

# FIXME: the board-status directory might get big over time. Is there a way we
# can push the results without fetching the whole repo?
coreboot_dir=`pwd`
if [ $UPLOAD_RESULTS -eq 1 ]; then
	# extract username from ssh://<username>@review.coreboot.org/blah
	username=$(git config --get remote.origin.url | sed 's/ssh\:\/\///' | sed 's/@.*//')

	cd "util/board_status/"
	if [ ! -e "board-status" ]; then
		git clone "ssh://${username}@review.coreboot.org:29418/board-status"
		if [ $? -ne 0 ]; then
			"Error cloning board-status repo, aborting."
			exit $EXIT_FAILURE
		fi
	fi

	cd "board-status"
	echo "Copying results to $(pwd)/${results}"

	# Note: Result directory should be unique due to the timestamp.
	cp -R "${tmpdir}/${vendor}" .

	echo "Uploading results"
	git add "${vendor}"
	git commit -a -am "${mainboard_dir}/${tagged_version}/${timestamp}"
	git push origin

	# Results have been uploaded so it's pointless to keep the
	# temporary files around.
	rm -rf "${tmpdir}"
fi
cd "$coreboot_dir"

if [ $CLOBBER_OUTPUT -eq 1 ]; then
	rm -rf ${tmpdir}
fi

exit $EXIT_SUCCESS
