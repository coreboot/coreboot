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


LBROOT=$( cd ../..; pwd )
TARGET=$( pwd )/linuxbios-builds

PAYLOAD=/usr/share/openbios/openbios.elf
PYTHON=python
MAKE="make"

ARCH=`uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc64/ \
	-e s/arm.*/arm/ -e s/sa110/arm/ -e s/x86_64/amd64/ \
	-e "s/Power Macintosh/ppc/"`

function vendors
{
	# make this a function so we can easily select
	# without breaking readability
	ls -1 $LBROOT/src/mainboard | grep -v CVS
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
	echo -n "  Creating config file..."
	mkdir -p $TARGET
	( cat << EOF
# This will make a target directory of ./VENDOR_MAINBOARD

target VENDOR_MAINBOARD
mainboard VENDOR/MAINBOARD

romimage "normal"
	option USE_FALLBACK_IMAGE=0
	option ROM_IMAGE_SIZE=0x10000
	option LINUXBIOS_EXTRA_VERSION=".0-normal"
	payload PAYLOAD
end

romimage "fallback" 
	option USE_FALLBACK_IMAGE=1
	option ROM_IMAGE_SIZE=0x10000
	option LINUXBIOS_EXTRA_VERSION=".0-fallback"
	payload PAYLOAD
end

buildrom ./VENDOR_MAINBOARD.rom ROM_SIZE "normal" "fallback"
EOF
	) | sed -e s,VENDOR,$VENDOR,g \
		-e s,MAINBOARD,$MAINBOARD,g \
		-e s,PAYLOAD,$PAYLOAD,g \
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
		echo "FAILED!"
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
		echo "ok."
		cd $CURR
	else
		echo "FAILED!"
		cd $CURR
		return 1
	fi
}

function build_target
{
	VENDOR=$1
	MAINBOARD=$2
	TARCH=$( architecture $VENDOR $MAINBOARD )
	
	echo -n "Processing mainboard $VENDOR $MAINBOARD"
	if [ "$ARCH" == "$TARCH" ]; then
		echo " ($TARCH: ok)"
		create_buildenv $VENDOR $MAINBOARD
		if [ $? -eq 0 ]; then
			compile_target $VENDOR $MAINBOARD
		fi
		echo
	else
		# cross compiling not supported yet.
		echo " ($TARCH: skipped, we're $ARCH)"
		echo
	fi
}

for VENDOR in $( vendors ); do
  for MAINBOARD in $( mainboards $VENDOR ); do
  	build_target $VENDOR $MAINBOARD
  done
done

