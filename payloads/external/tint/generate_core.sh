#!/bin/sh
#
# generate_core.sh extracts the core part from buildgcc script,
# most importantly the checksum calculation/verification functions.
#
# Copyright (C) 2019 Mike Banon <mikebdp2@gmail.com>
#
##############################################################################
#
# USAGE:
# 	./generate_core.sh <buildgcc> <corescript> prepare_before_patch
# where
# 	buildgcc   - path to input buildgcc script
# 	corescript - path to output core part script
# 	prepare_before_patch - optional argument to insert prepare_${package}
# 				call into the unpack_and_patch function, e.g.
# 				for removing some files with rm command
# 				in order to reduce the size of patch file
#
##############################################################################

buildgcc="$1"
corescript="$2"
prepare_before_patch="$3"

#
# Imports the source file fragment between start and end into the
# destination file, optionally excluding the last line if not needed
#

import_from_file() {
        source="$1"
        destination="$2"
	start="$3"
	end="$4"
	last_line_disabled="$5"
	if [ -z "${last_line_disabled}" ] ; then
		sed -n "/^${start}/,/^${end}/{/^${start}/{p;n};{p}}" "$source" >> "$destination"
	else
		sed -n "/^${start}/,/^${end}/{/^${start}/{p;n};/^${end}/{q};{p}}" "$source" >> "$destination"
	fi
}

#
# Import the color defines together with UNAME/HALT_FOR_TOOLS variables
#

import_from_file "$buildgcc" "$corescript" "red=" "HALT_FOR_TOOLS=0" || exit "$?"

#
# Import the core functions
#

FUNCTIONS="please_install searchtool download compute_hash error_hash_mismatch verify_hash unpack_and_patch"

for F in $FUNCTIONS ; do
	import_from_file "$buildgcc" "$corescript" "$F()" "}" || exit "$?"
done

#
# Import a fragment where we find tar/patch/make and other essential tools
#

import_from_file "$buildgcc" "$corescript" "# Find all the required tools" "# Allow" "last_line_disabled" || exit "$?"

#
# Import a fragment with conditional exit if some required tools were not found
#

import_from_file "$buildgcc" "$corescript" "if \[ \"\$HALT_FOR_TOOLS" "fi" || exit "$?"

#
# Avoid the unnecessary subdirectories holding a single file each
#

sed -i -e "s/patches\///g" "$corescript"
sed -i -e "s/sum\///g" "$corescript"
sed -i -e "s/tarballs\///g" "$corescript"
sed -i -e "s/cd tarballs || exit 1//g" "$corescript"
sed -i -e "s/cd \.\.//g" "$corescript"

#
# Get the known checksum without using a dedicated single-line file
#

sed -i -e "s/\tknown_hash=\"\$(get_known_hash.*/\tknown_hash=\"\$2\"/g" "$corescript"

#
# Update the paths printed at the error messages
#

sed -i -e "s/util\/crossgcc\///g" "$corescript"

#
# Insert prepare_${package} function call between the unpack and patch operations
#

if [ ! -z "${prepare_before_patch}" ] ; then
	sed -i -e "/\$TAR \$FLAGS \"\$(basename \"\$archive\")\"/a prepare_\${package} || exit \"\$?\"" "$corescript"
fi

#
