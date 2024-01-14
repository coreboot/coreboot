#!/usr/bin/env bash

# SPDX-License-Identifier: GPL-2.0-only

LANG=C
# Some tools emit errors that don't matter (bugs in lspci/PCI firmware and lsusb).
# To shown them anyway (e.g. for debugging) comment next line.
exec 2>/dev/null

if [ "$1" = "-h" ]; then
	printf "Usage: $0 [-h | path to dmesg log]

This script tries to find USB ports compatible with USB2/EHCI debug devices and
helps you to find their physical locations. To that end, attach at least one
uniquely identifiable device to a USB port and run this script. The device needs
to be visible in the output of \"lsusb -t\" (debug devices are often *not*!).

After determining compatibility of the USB controllers the script will print the
devices attached to the debug port as shown by lsusb. If nothing shows up simply
switch ports and repeat the process.

Note: usually only one port is supported for debugging.\n"
	exit 0
fi
uid=`id -u`
if [ "$uid" -ne 0 ]; then
	echo "Must be run as root. Exiting."
	exit 1
fi

if ! command -v lsusb; then
	echo "lsusb not found. Please install \"usbutils\" from your
distribution's package manager. Exiting."
	exit 1
fi

if ! command -v lspci; then
	echo "lspci not found. Please install \"pciutils\" from your
distribution's package manager. Exiting."
	exit 1
fi

dmesgfile=$1

find_devs_in_tree () {
	bus=$1
	port=$2

	# lsusb -t uses 3 digits for bus/port nunmbers as of version 016 and later
        if [ $(lsusb -V | cut -f 3 -d " ") -lt 16 ]; then
		busstr=`printf "Bus %02d" "$bus"`
		portstr="Port $port"
	else
		busstr=`printf "Bus %03d" "$bus"`
		portstr=`printf "Port %03d" "$port"`
	fi

	hubs_to_ignore="8087:0020 8087:0024 8087:8000 8087:8008"
	reqlvl=1

	found=
	# Iterate over the output of lsusb -t because it contains the physical port numbers
	while IFS='' read -r line; do
		# We need to keep track of the current bus "branch"
		# Look out for lines starting with /: (that indicate a bus)
		if [ "${line#*/:}" != "$line" ]; then
			if [ "${line#*$busstr}" != "$line" ]; then
				cur_bus=$busstr
			else
				cur_bus=
			fi
			continue
		fi

		# Skip all lines not belonging to the wanted bus number
		if [ "$cur_bus" != "$busstr" ]; then
			continue
		fi

		# Calculate current USB tier/level
		spaces="${line%%[!' ']*}"
		curlvl=$((${#spaces} / 4))
		if [ $curlvl -ne $reqlvl ]; then
			continue
		fi

		# Fetch USB IDs of the current device
		dev=`echo ${line#*Dev } | cut -d ',' -f 1`
		lsusbline=`lsusb -s "$bus":"$dev"`
		if [[ ! "$lsusbline" =~ .*([[:xdigit:]]{4}:[[:xdigit:]]{4}) ]]; then
			printf "Unexpected output from \"%s\": \"%s\"\n" "lsusb -s $bus:$dev" "$usbline"
			exit 1
		fi
		ids=${BASH_REMATCH[1]}

		# Skip over rate matching hubs
		if [[ "$hubs_to_ignore" == *"$ids"* ]]; then
			((reqlvl += 1))
			continue
		fi

		# Check for matching physical USB port
		if [ "${line#*$portstr}" != "$line" ]; then
			echo "$lsusbline"
			return
		fi
	done<< EOF
$(lsusb -t)
EOF
	if [ -z "$found" ]; then
		echo "none"
	fi
}

debug_lspci_devs=`lspci -nvvD |
	grep -i "^[0-9a-f]\|debug port" |
	grep -iB1 --no-group-separator "debug port" |
	grep -vi "debug port" |
	cut -f 1 -d" " |
	sort |
	xargs echo`

if [ -z "$debug_lspci_devs" ]; then
	printf "No USB controller with debug capability found by lspci.\n
Possible reasons: lspci too old, USB controller does not support a debug device, ... Exiting.\n"
	exit 1
fi
printf "The following PCI devices support a USB debug port (says lspci): $debug_lspci_devs\n"

debug_dmesg_devs_with_port=`( test -z "$dmesgfile" &&
	dmesg ||
	cat "$dmesgfile") |
	grep -i "ehci.*debug port" |
	sed "s/.* \([0-9a-f]*:*[0-9a-f]\{2\}:[0-9a-f]\{2\}\.[0-9a-f]\).*ebug port /\1 /" |
	sort`

debug_dmesg_devs=`echo "$debug_dmesg_devs_with_port" |
	cut -f 1 -d" " |
	xargs echo`

if [ -z "$debug_dmesg_devs" ]; then
	printf "dmesg does not show any supported ports.\n
Possible reasons: dmesg scrolled off, kernel too old, USB controller does not support a debug device, ... Exiting.\n
Note: You can specify a file containing kernel messages as an argument to this program (e.g. /var/log/dmesg)."
	exit 1
fi

if [ "$debug_lspci_devs" != "$debug_dmesg_devs" ]; then
	echo "lspci and the kernel do not agree on USB debug device support. Exiting."
	exit 1
fi

printf "and the kernel agrees, good.\n\n"

while true; do
	for dev in $debug_dmesg_devs; do
		bus=`lsusb -v |
			grep "^Bus\|iSerial.*" |
			grep -B1 --no-group-separator "iSerial.*$dev" |
			grep "^Bus" |
			sed "s/Bus *0*\([0-9a-f]*\).*/\1/"`
		port=`echo "$debug_dmesg_devs_with_port" |
			grep "^$dev" |
			cut -f 2 -d" "`

		echo "Device(s) currently connected to the debug-capable port $port on PCI device $dev, USB bus $bus:"

		find_devs_in_tree "$bus" "$port"
		echo
	done

	echo "Enter 'q' to abort or anything else to repeat"
	read -r r
	if [ $? -ne 0 -o "$r" = "q" ]; then
		break;
	fi
done

exit 0
