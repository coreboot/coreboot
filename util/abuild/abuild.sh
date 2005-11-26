#!/bin/bash
#
#  LinuxBIOS autobuild
#
#  This script builds LinuxBIOS images for all available targets.
#
#  (C) 2004 by Stefan Reinauer <stepan@openbios.org>
#
#  This file is subject to the terms and conditions of the GNU General
#  Public License. See the file COPYING in the main directory of this
#  archive for more details.
#     

#set -x # Turn echo on....

# Where shall we place all the build trees?
TARGET=$( pwd )/linuxbios-builds

# path to payload. Should be more generic
PAYLOAD=/dev/null

# Lines of error context to be printed in FAILURE case
CONTEXT=5

# One might want to adjust these in case of cross compiling
MAKE="make"
PYTHON=python

ARCH=`uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc64/ \
	-e s/arm.*/arm/ -e s/sa110/arm/ -e s/x86_64/amd64/ \
	-e "s/Power Macintosh/ppc/"`

function debug
{
	test "$verbose" == "true" && echo $*
}

function vendors
{
	# make this a function so we can easily select
	# without breaking readability
	ls -1 "$LBROOT/src/mainboard" | grep -v CVS
}

function mainboards
{
	# make this a function so we can easily select
	# without breaking readability
	
	VENDOR=$1
	
	ls -1 $LBROOT/src/mainboard/$VENDOR | grep -v CVS 
}

function architecture
{
	VENDOR=$1
	MAINBOARD=$2
	cat $LBROOT/src/mainboard/$VENDOR/$MAINBOARD/Config.lb | \
		grep ^arch | cut -f 2 -d\ 
}

function create_config
{
	VENDOR=$1
	MAINBOARD=$2
	TARCH=$( architecture $VENDOR $MAINBOARD )
	TARGCONFIG=$LBROOT/targets/$VENDOR/$MAINBOARD/Config-abuild.lb

	mkdir -p $TARGET

        if [ -f $TARGCONFIG ]; then
        	cp $TARGCONFIG $TARGET/Config-${VENDOR}_${MAINBOARD}.lb
		echo "Used existing test target $TARGCONFIG"
		return 
	fi

	echo -n "  Creating config file..."
	( cat << EOF
# This will make a target directory of ./VENDOR_MAINBOARD

target VENDOR_MAINBOARD
mainboard VENDOR/MAINBOARD

option CC="CROSSCC"
option CROSS_COMPILE="CROSS_PREFIX"
option HOSTCC="CROSS_HOSTCC"

EOF
	if [ $TARCH == i386 ] ; then
		cat <<EOF
romimage "normal"
	option USE_FALLBACK_IMAGE=0
	option ROM_IMAGE_SIZE=0x18000
	option LINUXBIOS_EXTRA_VERSION=".0-normal"
	payload PAYLOAD
end

romimage "fallback" 
	option USE_FALLBACK_IMAGE=1
	option ROM_IMAGE_SIZE=0x18000
	option LINUXBIOS_EXTRA_VERSION=".0-fallback"
	payload PAYLOAD
end
buildrom ./linuxbios.rom ROM_SIZE "normal" "fallback"
EOF
	else
		cat <<EOF
romimage "only"
	option LINUXBIOS_EXTRA_VERSION=".0"
	payload PAYLOAD
end
buildrom ./linuxbios.rom ROM_SIZE "only"
EOF
	fi
	) | sed -e s,VENDOR,$VENDOR,g \
		-e s,MAINBOARD,$MAINBOARD,g \
		-e s,PAYLOAD,$PAYLOAD,g \
		-e s,CROSSCC,"$CC",g \
		-e s,CROSS_PREFIX,"$CROSS_COMPILE",g \
		-e s,CROSS_HOSTCC,"$HOSTCC",g \
		> $TARGET/Config-${VENDOR}_${MAINBOARD}.lb
	echo " ok"
}

function create_builddir
{	
	VENDOR=$1
	MAINBOARD=$2
	
	echo -n "  Creating builddir..."

	target_dir=$TARGET
	config_dir=$LBROOT/util/newconfig
	yapps2_py=$config_dir/yapps2.py
	config_g=$config_dir/config.g
	config_lb=Config-${VENDOR}_${MAINBOARD}.lb

	cd $target_dir

	build_dir=${VENDOR}_${MAINBOARD}
	config_py=$build_dir/config.py

	if [ ! -d $build_dir ] ; then
		mkdir -p $build_dir
	fi
	if [ ! -f $config_py ]; then
		$PYTHON $yapps2_py $config_g $config_py &> $build_dir/py.log
	fi

	# make sure config.py is up-to-date

	export PYTHONPATH=$config_dir
	$PYTHON $config_py $config_lb $LBROOT &> $build_dir/config.log
	if [ $? -eq 0 ]; then
		echo "ok"
	else
		echo "FAILED! Log excerpt:"
		tail -n $CONTEXT $build_dir/config.log
		return 1
	fi
}

function create_buildenv
{
	VENDOR=$1
	MAINBOARD=$2
	create_config $VENDOR $MAINBOARD
	create_builddir $VENDOR $MAINBOARD
}

function compile_target
{	
	VENDOR=$1
	MAINBOARD=$2

	echo -n "  Compiling image .."
	CURR=$( pwd )
	cd $TARGET/${VENDOR}_${MAINBOARD}
	eval $MAKE &> make.log
	if [ $? -eq 0 ]; then
		echo "ok" > compile.status
		echo "ok."
		cd $CURR
		return 0
	else
		echo "FAILED! Log excerpt:"
		tail -n $CONTEXT make.log
		cd $CURR
		return 1
	fi
}

function built_successfully
{
	CURR=`pwd`
	status="fail"
	if [ -d "$TARGET/${VENDOR}_${MAINBOARD}" ]; then
		cd $TARGET/${VENDOR}_${MAINBOARD}
		if [ -r compile.status ] ; then
			status=`cat compile.status`
		fi
		cd $CURR
	fi
	[ "$buildall" != "true" -a "$status" == "ok" ]
}

function build_broken
{
	CURR=`pwd`
	status="yes"
	[ -r "$LBROOT/src/mainboard/${VENDOR}/${MAINBOARD}/BROKEN" ] && status="no"
	[ "$buildbroken" == "true" -o "$status" == "yes" ]
}

function build_target
{
	VENDOR=$1
	MAINBOARD=$2
	TARCH=$( architecture $VENDOR $MAINBOARD )

	# default setting
	CC='$(CROSS_COMPILE)gcc'
	HOSTCC='gcc'
	CROSS_COMPILE=''

	echo -n "Processing mainboard/$VENDOR/$MAINBOARD"
	
	if [ "$ARCH" == "$TARCH" ]; then
		echo " ($TARCH: ok)"
	else
		found_crosscompiler=false
		if [ "$ARCH" == amd64 -a "$TARCH" == i386 ]; then
			CC="gcc -m32"
			found_crosscompiler=true
		fi
		if [ "$ARCH" == ppc64 -a "$TARCH" == ppc ]; then
			CC="gcc -m32"
			found_crosscompiler=true
		fi
		if [ "$found_crosscompiler" == "false" -a "$TARCH" == ppc ];then
			for prefix in powerpc-eabi- powerpc-linux- ppc_74xx- \
			    powerpc-7450-linux-gnu-; do
				if ${prefix}gcc --version > /dev/null 2> /dev/null ; then
					found_crosscompiler=true
					CROSS_COMPILE=$prefix
				fi
			done
		fi

		# TBD: look for suitable cross compiler suite
		# cross-$TARCH-gcc and cross-$TARCH-ld
		
		# Check result:
		if [ $found_crosscompiler == "false" ]; then
			echo " ($TARCH: skipped, we're $ARCH)"
			echo
			return 0
		else
			echo " ($TARCH: ok, we're $ARCH)"
		fi
	fi

	built_successfully $VENDOR $MAINBOARD && \
	{
		echo " ( mainboard/$VENDOR/$MAINBOARD previously ok )"
		echo
		return 0
	}

	build_broken $VENDOR $MAINBOARD || \
	{
		echo " ( broken mainboard/$VENDOR/$MAINBOARD skipped )"
		echo
		return 0
	}
	
	create_buildenv $VENDOR $MAINBOARD
	if [ $? -eq 0 ]; then
		compile_target $VENDOR $MAINBOARD
	fi

	echo
}

function myhelp
{
	echo "Usage: $0 [-v] [-a] [-b] [-t <vendor/board>] [lbroot]"
	echo "       $0 [-V|--version]"
	echo "       $0 [-h|--help]"
	echo
	echo "Options:"
	echo "    [-v|--verbose]		  print more messages"
	echo "    [-a|--all]			  build previously succeeded ports as well"
	echo "    [-b|--broken]		  attempt to build ports that are known broken"
	echo "    [-t|--target <vendor/board>]  attempt to build target vendor/board only"
	echo "    [-V|--version]		  print version number and exit"
	echo "    [-h|--help]			  print this help and exit"
	echo "    [lbroot]			  absolute path to LinuxBIOS sources"
	echo "				  (defaults to $LBROOT)"
	echo
}

function myversion 
{
	cat << EOF

LinuxBIOS autobuild: V0.1.

Copyright (C) 2004 by Stefan Reinauer, <stepan@openbios.org>
This program is free software; you may redistribute it under the terms
of the GNU General Public License. This program has absolutely no
warranty.

EOF
	myhelp
}

# default options
target=""
buildall=false
LBROOT=$( cd ../..; pwd )
verbose=false

# parse parameters
args=`getopt -l version,verbose,help,all,target:,broken Vvhat:b -- "$@"`

if [ $? != 0 ]; then
	myhelp
	exit 1
fi

eval set "$args"
while true ; do
	case "$1" in
		-t|--target)	shift; target="$1"; shift;;
		-a|--all)	shift; buildall=true;;
		-b|--broken)	shift; buildbroken=true;;
		-v|--verbose)	shift; verbose=true;;
		-V|--version)	shift; myversion; exit 0;;
		-h|--help)	shift; myhelp; exit 0;;
		--)		shift; break;;
		-*)		echo -e "Invalid option\n"; myhelp; exit 1;;
		*)		break;;
	esac
done

# /path/to/freebios2/
test -z "$1" || LBROOT=$1

debug "LBROOT=$LBROOT"

if [ "$target" != "" ]; then
	# build a single board
	VENDOR=`echo $target|cut -f1 -d/`
	MAINBOARD=`echo $target|cut -f2 -d/`
	build_target $VENDOR $MAINBOARD
else
	# build all boards per default
	for VENDOR in $( vendors ); do
		for MAINBOARD in $( mainboards $VENDOR ); do
			build_target $VENDOR $MAINBOARD
		done
	done
fi

