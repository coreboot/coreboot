#!/bin/bash
#
# This file is part of the coreboot project.
#
# Copyright (C) 2007-2010 coresystems GmbH
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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#

MICROCODE_VERSION=20130222
MICROCODE_ARCHIVE=microcode-$MICROCODE_VERSION.tgz
MICROCODE_FILE=microcode.dat
INTEL_MICROCODE=http://downloadmirror.intel.com/22508/eng/$MICROCODE_ARCHIVE

#
# Getting Intel(R) Microcode
#

get_microcode() {
    printf "Getting microcode...\n"
    wget -nv $INTEL_MICROCODE
    tar xzf $MICROCODE_ARCHIVE
}

#
# Creating separate files per microcode
#

separate_microcode() {
    printf "Separating microcode...\n"
    csplit -s -n4 -k $MICROCODE_FILE '/^\/\*.*\.inc.*\*\//' '{500}' 2> /dev/null
    mv xx0000 header.inc
    perl -pi -e 's,\ \ \ \ \ \ \ ,\	,' header.inc
    perl -pi -e 's,^,/,g' header.inc
    perl -pi -e 's,^//\*,/\*,' header.inc
    for i in xx????; do
        name="`head -1 $i`"
        name=${name%??}
        name=${name:2}
        name=$( echo $name )
        name=microcode-${name%.inc}.h
        cat header.inc $i > $name
    done
    rm -f xx???? header.inc
}

#
# Dump CPUIDs from all separated files
#

dump_cpuids() {
    ls -1 microcode-*.h | while read F; do
	CPUID="$( echo $( head -36 $F |tail -1|cut -d, -f4|sed s,0x,, ) | sed 's/0*//')"
	echo "$CPUID:$F"
    done
}

#
# Move microcode to target positions
#

move_microcode() {
    printf "Moving microcode...\n"
    dump_cpuids | sort | while read N; do
        ID=$( echo $N | cut -d: -f1 )
	F=$( echo $N | cut -d: -f2 )

	if [ -d ../model_$ID ]; then
	    echo "Model: $ID  Microcode: $F"
	    mv $F ../model_$ID/$F
	else
	    ID2=${ID%?}x
	    if [ -d ../model_$ID2 ]; then
	        echo "Model: $ID($ID2)  Microcode: $F (copied)"
		mv $F ../model_$ID2/$F
            else
	        ID1=${ID%??}xx
		if [ -d ../model_$ID1 ]; then
	            echo "Model: $ID($ID1)  Microcode: $F (copied)"
		    mv $F ../model_$ID1/$F
		else
	            echo "Model: $ID  Microcode: $F (erased)"
		    rm -f $F
		fi
	    fi
	fi
    done
}

get_microcode
separate_microcode
move_microcode

rm -f $MICROCODE_ARCHIVE
rm -f $MICROCODE_FILE

