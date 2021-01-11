#!/usr/bin/env sh
#
#

EXIT_SUCCESS=0
EXIT_FAILURE=1

# Stuff from command-line switches
COREBOOT_IMAGE="build/coreboot.rom"
REMOTE_HOST=""
REMOTE_PORT_OPTION=""
CLOBBER_OUTPUT=0
UPLOAD_RESULTS=0
SERIAL_PORT_SPEED=115200

# Used to specify whether a command should always be run locally or
# if command should be run remoteley when a remote host is specified.
LOCAL=0
REMOTE=1
FATAL=0
NONFATAL=1

# Used if cbmem is not in default $PATH, e.g. not installed or when using `sudo`
CBMEM_PATH=""

# Used if nvramtool is not in default $PATH, e.g. not installed or when using `sudo`
NVRAMTOOL_PATH=""

case $(uname) in
	FreeBSD)
		if [ ! -x /usr/local/bin/gmake ]; then
			echo "Please install gmake, or build and install devel/gmake from ports."
			exit $EXIT_FAILURE
		else
			MAKE=gmake
		fi
		;;
	*)
		MAKE=make
		;;
esac

# test a command
#
# $1: 0 ($LOCAL) to run command locally,
#     1 ($REMOTE) to run remotely if remote host defined
# $2: command to test
# $3: 0 ($FATAL) Exit with an error if the command fails
#     1 ($NONFATAL) Don't exit on command test failure
test_cmd()
{
	local rc

	if [ -e "$2" ]; then
		return
	fi

	if [ "$1" -eq "$REMOTE" ] && [ -n "$REMOTE_HOST" ]; then
		ssh $REMOTE_PORT_OPTION root@${REMOTE_HOST} command -v "$2" > /dev/null
		rc=$?
	else
		command -v "$2" >/dev/null
		rc=$?
	fi

	if [ $rc -eq 0 ]; then
		return 0
	fi

	if [ "$3" = "1" ]; then
		return 1
	fi

	echo "$2 not found"
	exit $EXIT_FAILURE
}

_cmd()
{
	if [ -e "$2" ]; then
		return $EXIT_FAILURE
	fi

	if [ -n "$3" ]; then
		pipe_location="${3}"
	else
		pipe_location="/dev/null"
	fi

	if [ "$1" -eq "$REMOTE" ] && [ -n "$REMOTE_HOST" ]; then
		ssh $REMOTE_PORT_OPTION "root@${REMOTE_HOST}" "$2" > "$pipe_location" 2>&1
	else
		$2 > "$pipe_location" 2>&1
	fi

	return $?
}

# run a command
#
# $1: 0 ($LOCAL) to run command locally,
#     1 ($REMOTE) to run remotely if remote host defined
# $2: command
# $3: filename to direct output of command into
cmd()
{
	_cmd $1 "$2" "$3"

	if [ $? -eq 0 ]; then
		return
	fi

	echo "Failed to run \"$2\", aborting"
	rm -f "$3"	# don't leave an empty file
	exit $EXIT_FAILURE
}

# run a command where failure is considered to be non-fatal
#
# $1: 0 ($LOCAL) to run command locally,
#     1 ($REMOTE) to run remotely if remote host defined
# $2: command
# $3: filename to direct output of command into
cmd_nonfatal()
{
	_cmd $1 "$2" "$3"

	if [ $? -eq 0 ]; then
		return
	fi

	echo "Failed to run \"$2\", ignoring"
	rm -f "$3"	# don't leave an empty file
}

# read from a serial port device
#
# $1: serial device to read from
# $2: serial port speed
# $3: filename to direct output of command into
get_serial_bootlog () {

	local TTY=$1
	local SPEED=$2
	local FILENAME=$3

	if [ ! -c "$TTY" ]; then
		echo "$TTY is not a valid serial device"
		exit $EXIT_FAILURE
	fi

	# make the text more noticible
	test_cmd $LOCAL "tput" $NONFATAL
	tput_not_available=$?
	if [ $tput_not_available -eq 0 ]; then
		tput bold
		tput setaf 10 # set bright green
	fi

	echo
	echo "Waiting to receive boot log from $TTY"
	echo "Press [Enter] when the boot is complete."
	echo

	if [ $tput_not_available -eq 0 ]; then
		tput sgr0
	fi

	# set up the serial port
	stty -F $TTY $SPEED cs8 -cstopb -parenb clocal

	# read from the serial port - user must press enter when complete
	test_cmd $LOCAL "tee"
	while read LINE; do
		echo "$LINE" | tee -a "$FILENAME"
	done < "$SERIAL_DEVICE" &
	PID=$!

	read foo
	kill "$PID" 2>/dev/null

	echo "Finished reading boot log."
}

show_help() {
	echo "Usage:
	${0} <option>

Options
    -c, --cbmem
        Path to cbmem on device under test (DUT).
    -n, --nvramtool
        Path to nvramtool on device under test (DUT).
    -C, --clobber
        Clobber temporary output when finished. Useful for debugging.
    -h, --help
        Show this message.
    -i, --image  <image>
        Path to coreboot image (Default is $COREBOOT_IMAGE).
    -r, --remote-host  <host>
        Obtain machine information from remote host (using ssh).
    -s, --serial-device  </dev/xxx>
        Obtain boot log via serial device.
    -S, --serial-speed  <speed>
        Set the port speed for the serial device (Default is $SERIAL_PORT_SPEED).
    -u, --upload-results
        Upload results to coreboot.org.

Long options:
    --ssh-port <port>
        Use a specific SSH port.
"
}

case $(uname) in
	FreeBSD)
		if [ ! -x /usr/local/bin/getopt ]; then
			echo "Please install getopt, or build and install misc/getopt from ports."
			exit $EXIT_FAILURE
		else
			GETOPT=/usr/local/bin/getopt
		fi
		;;
	*)
	GETOPT=/usr/bin/getopt
	;;
esac

$GETOPT -T
if [ $? -ne 4 ]; then
	echo "GNU-compatible getopt(1) required."
	exit $EXIT_FAILURE
fi

LONGOPTS="cbmem:,clobber,help,image:,remote-host:,upload-results"
LONGOPTS="${LONGOPTS},serial-device:,serial-speed:"
LONGOPTS="${LONGOPTS},ssh-port:"

ARGS=$($GETOPT -o c:n:Chi:r:s:S:u -l "$LONGOPTS" -n "$0" -- "$@");
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi
eval set -- "$ARGS"
while true ; do
	case "$1" in
		# generic options
		-c|--cbmem)
			shift
			CBMEM_PATH="$1"
			;;
		-n|--nvramtool)
			shift
			NVRAMTOOL_PATH="$1"
			;;
		-C|--clobber)
			CLOBBER_OUTPUT=1
			;;
		-h|--help)
			show_help
			exit $EXIT_SUCCESS
			;;
		-i|--image)
			shift
			COREBOOT_IMAGE="$1"
			;;
		-r|--remote-host)
			shift
			REMOTE_HOST="$1"
			;;
		-u|--upload-results)
			UPLOAD_RESULTS=1
			;;

		# serial port options
		-s|--serial-device)
			shift
			SERIAL_DEVICE="$1"
			;;
		-S|--serial-speed)
			shift
			SERIAL_PORT_SPEED="$1"
			;;

		# ssh options
		--ssh-port)
			shift
			REMOTE_PORT_OPTION="-p $1"
			;;

		# error handling
		--)
			shift
			if [ -n "$*" ]; then
				echo "Non-option parameters detected: '$*'"
				exit $EXIT_FAILURE
			fi
			break
			;;
		*)
			echo "error processing options at '$1'"
			exit $EXIT_FAILURE
	esac
	shift
done

grep -rH 'coreboot.org' .git/config >/dev/null 2>&1
if [ $? -ne 0 ]; then
	echo "Script must be run from root of coreboot directory"
	exit $EXIT_FAILURE
fi

if [ ! -e "$COREBOOT_IMAGE" ]; then
	echo "board_status needs $COREBOOT_IMAGE, but it does not exist."
	echo "Use \"-i IMAGE_FILE\" to select a different image, or \"--help\" for more options."
	exit $EXIT_FAILURE
fi

# Results will be placed in a temporary location until we're ready to upload.
# If the user does not wish to upload, results will remain in /tmp.
case $(uname) in
	FreeBSD)
		tmpdir=$(mktemp -d -t coreboot_board_status)
		;;
	*)
		tmpdir=$(mktemp -d --tmpdir coreboot_board_status.XXXXXXXX)
		;;
esac

# Obtain coreboot config by running cbfstool on the ROM image. cbfstool may
# already exist in build/ or util/cbfstool/, but if not then we'll build it
# now and clean it when we're done.
cbfstool_cmd="build/cbfstool"
do_clean_cbfstool=0
if [ ! -x $cbfstool_cmd ]; then
	cbfstool_cmd="util/cbfstool/cbfstool"
	if [ -e $cbfstool_cmd ]; then
		if test ! -x $cbfstool_cmd; then
			echo "Cannot execute $cbfstool_cmd."
			exit $EXIT_FAILURE
		fi
	else
		$MAKE -C util/cbfstool/
		do_clean_cbfstool=1
	fi
fi
test_cmd $LOCAL "$cbfstool_cmd"

tmpcfg=$(mktemp coreboot_config.XXXXXX)
echo "Extracting config.txt from $COREBOOT_IMAGE"
$cbfstool_cmd "$COREBOOT_IMAGE" extract -n config -f "${tmpdir}/config.txt" >/dev/null 2>&1
mv "${tmpdir}/config.txt" "${tmpdir}/config.short.txt"
cp "${tmpdir}/config.short.txt" "${tmpcfg}"
yes "" | $MAKE "DOTCONFIG=${tmpcfg}" oldconfig 2>/dev/null >/dev/null
mv "${tmpcfg}" "${tmpdir}/config.txt"
rm -f "${tmpcfg}.old"
$cbfstool_cmd "$COREBOOT_IMAGE" print > "${tmpdir}/cbfs.txt"
rom_contents=$($cbfstool_cmd "$COREBOOT_IMAGE" print 2>&1)
if [ -n "$(echo $rom_contents | grep payload_config)" ]; then
	echo "Extracting payload_config from $COREBOOT_IMAGE"
	$cbfstool_cmd "$COREBOOT_IMAGE" extract -n payload_config -f "${tmpdir}/payload_config.txt" >/dev/null 2>&1
fi
if [ -n "$(echo $rom_contents | grep payload_version)" ]; then
	echo "Extracting payload_version from $COREBOOT_IMAGE"
	$cbfstool_cmd "$COREBOOT_IMAGE" extract -n payload_version -f "${tmpdir}/payload_version.txt" >/dev/null 2>&1
fi
md5sum -b "$COREBOOT_IMAGE" > "${tmpdir}/rom_checksum.txt"

if test $do_clean_cbfstool -eq 1; then
	$MAKE -C util/cbfstool clean
fi

# Obtain board and revision info to form the directory structure:
# <vendor>/<board>/<revision>/<timestamp>
mainboard_dir="$(grep CONFIG_MAINBOARD_DIR "${tmpdir}/config.txt" | awk -F '"' '{ print $2 }')"
vendor=$(echo "$mainboard_dir" | awk -F '/' '{ print $1 }')
mainboard=$(echo "$mainboard_dir" | awk -F '/' '{ print $2 }')

getrevision="util/board_status/getrevision.sh"
test_cmd $LOCAL $getrevision
tagged_version=$($getrevision -T)
timestamp=$($getrevision -t)

results="${vendor}/${mainboard}/${tagged_version}/${timestamp}"

if [ -n "$(echo $tagged_version | grep dirty)" ]; then
	echo "The repository is in a dirty state. Please see the output of"
	echo "'git status' below."
	git status
	exit $EXIT_FAILURE
fi

echo "Temporarily placing output in ${tmpdir}/${results}"
mkdir -p "${tmpdir}/${results}"

mv "${tmpdir}/config.txt" "${tmpdir}/${results}"
test -f "${tmpdir}/payload_config.txt" && mv "${tmpdir}/payload_config.txt" "${tmpdir}/${results}"
test -f "${tmpdir}/payload_version.txt" && mv "${tmpdir}/payload_version.txt" "${tmpdir}/${results}"
mv "${tmpdir}/config.short.txt" "${tmpdir}/${results}"
mv "${tmpdir}/cbfs.txt" "${tmpdir}/${results}"
mv "${tmpdir}/rom_checksum.txt" "${tmpdir}/${results}"

touch "${tmpdir}/${results}/revision.txt"
printf "Local revision: %s\n" "$($getrevision -l)" >> "${tmpdir}/${results}/revision.txt"
printf "Tagged revision: %s\n" "${tagged_version}" >> "${tmpdir}/${results}/revision.txt"
printf "Upstream revision: %s\n" "$($getrevision -u)" >> "${tmpdir}/${results}/revision.txt"
printf "Upstream URL: %s\n" "$($getrevision -U)" >> "${tmpdir}/${results}/revision.txt"
printf "Timestamp: %s\n" "$timestamp" >> "${tmpdir}/${results}/revision.txt"

if [ -n "$CBMEM_PATH" ]; then
	cbmem_cmd="$CBMEM_PATH"
else
	cbmem_cmd="cbmem"
fi

cmos_enabled=0
if grep -q "CONFIG_USE_OPTION_TABLE=y" "${tmpdir}/${results}/config.short.txt" > /dev/null; then
	cmos_enabled=1
fi

if [ -n "$NVRAMTOOL_PATH" ]; then
	nvramtool_cmd="$NVRAMTOOL_PATH"
else
	nvramtool_cmd="nvramtool"
fi

if [ -n "$SERIAL_DEVICE" ]; then
	get_serial_bootlog "$SERIAL_DEVICE" "$SERIAL_PORT_SPEED" "${tmpdir}/${results}/coreboot_console.txt"
elif [ -n "$REMOTE_HOST" ]; then
	echo "Verifying that CBMEM is available on remote device"
	test_cmd $REMOTE "$cbmem_cmd"
	echo "Getting coreboot boot log"
	cmd $REMOTE "$cbmem_cmd -1" "${tmpdir}/${results}/coreboot_console.txt"
	echo "Getting timestamp data"
	cmd_nonfatal $REMOTE "$cbmem_cmd -t" "${tmpdir}/${results}/coreboot_timestamps.txt"

	if [ "$cmos_enabled" -eq 1 ]; then
		echo "Verifying that nvramtool is available on remote device"
		test_cmd $REMOTE "$nvramtool_cmd"
		echo "Getting all CMOS values"
		cmd $REMOTE "$nvramtool_cmd -a" "${tmpdir}/${results}/cmos_values.txt"
	fi

	echo "Getting remote dmesg"
	cmd $REMOTE dmesg "${tmpdir}/${results}/kernel_log.txt"
else
	echo "Verifying that CBMEM is available"
	if [ $(id -u) -ne 0 ]; then
		command -v "$cbmem_cmd" >/dev/null
		if [ $? -ne 0 ]; then
			echo "Failed to run $cbmem_cmd. Check \$PATH or" \
			"use -c to specify path to cbmem binary."
			exit $EXIT_FAILURE
		else
			cbmem_cmd="sudo $cbmem_cmd"
		fi
	else
		test_cmd $LOCAL "$cbmem_cmd"
	fi

	echo "Getting coreboot boot log"
	cmd $LOCAL "$cbmem_cmd -1" "${tmpdir}/${results}/coreboot_console.txt"

	echo "Getting timestamp data"
	cmd_nonfatal $LOCAL "$cbmem_cmd -t" "${tmpdir}/${results}/coreboot_timestamps.txt"

	if [ "$cmos_enabled" -eq 1 ]; then
		echo "Verifying that nvramtool is available"
		if [ $(id -u) -ne 0 ]; then
			command -v "$nvramtool_cmd" >/dev/null
			if [ $? -ne 0 ]; then
				echo "Failed to run $nvramtool_cmd. Check \$PATH or" \
				"use -n to specify path to nvramtool binary."
				exit $EXIT_FAILURE
			else
				nvramtool_cmd="sudo $nvramtool_cmd"
			fi
		else
			test_cmd $LOCAL "$nvramtool_cmd"
		fi

		echo "Getting all CMOS values"
		cmd $LOCAL "$nvramtool_cmd -a" "${tmpdir}/${results}/cmos_values.txt"
	fi

	echo "Getting local dmesg"
	cmd $LOCAL "sudo dmesg" "${tmpdir}/${results}/kernel_log.txt"
fi

#
# Check files
#
if [ $(grep -- -dirty "${tmpdir}/${results}/coreboot_console.txt") ]; then
	echo "coreboot or the payload are built from a source tree in a" \
	"dirty state, making it hard to reproduce the result. Please" \
	"check in your source tree with 'git status'."
	exit $EXIT_FAILURE
fi

if [ $(grep -- unknown "${tmpdir}/${results}/coreboot_timestamps.txt") ]; then
	echo "Unknown timestamps found in 'coreboot_timestamps.txt'." \
	"Please rebuild the 'cbmem' utility and try again."
	exit $EXIT_FAILURE
fi

#
# Finish up.
#
coreboot_dir=$(pwd)
if [ $UPLOAD_RESULTS -eq 1 ]; then
	# extract username from ssh://<username>@review.coreboot.org/blah
	bsrepo=$(git config --get remote.origin.url | sed "s,\(.*\)/coreboot,\1/board-status,")

	cd "util/board_status/"
	if [ ! -e "board-status" ]; then
		# FIXME: the board-status directory might get big over time.
		# Is there a way we can push the results without fetching the
		# whole repo?
		git clone "$bsrepo"
		if [ $? -ne 0 ]; then
			echo "Error cloning board-status repo, aborting."
			exit $EXIT_FAILURE
		fi
	fi

	cd "board-status"

	echo "Checking for duplicate results"
	# get any updates to board-status
	git pull

	echo "${tagged_version}" | grep dirty >/dev/null 2>&1
	clean_version=$?
	existing_results=$(git ls-files "${mainboard_dir}/${tagged_version}")

	# reject duplicate results of non-dirty versions
	if [ "${clean_version}" -eq 1 ] && [ -n "${existing_results}" ] ; then
		echo "Result is a duplicate, aborting"
		exit $EXIT_FAILURE
	fi

	echo "Copying results to $(pwd)/${results}"

	# Note: Result directory should be unique due to the timestamp.
	cp -R "${tmpdir}/${vendor}" .

	echo "Uploading results"
	git add "${vendor}"
	git commit -a -m "${mainboard_dir}/${tagged_version}/${timestamp}"
	count=0
	until git push origin master || test $count -eq 3; do
	        git pull --rebase
		count=$((count + 1))
	done

	# Results have been uploaded so it's pointless to keep the
	# temporary files around.
	rm -rf "${tmpdir}"
	if test $count -eq 3; then
		echo "Error uploading to board-status repo, aborting."
		exit $EXIT_FAILURE
	fi
fi
cd "$coreboot_dir"

if [ $CLOBBER_OUTPUT -eq 1 ]; then
	rm -rf "${tmpdir}"
else
	if [ $UPLOAD_RESULTS -eq 1 ]; then
		echo
		echo "output files are in $(dirname $0)/board-status/${mainboard_dir}/${tagged_version}/${timestamp}"
	else
		echo
		echo "output files are in ${tmpdir}/${results}"
	fi
fi

exit $EXIT_SUCCESS
