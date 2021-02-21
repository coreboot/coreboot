#!/bin/sh
#
# tintify_core.sh adds the TINT-specific footer/header to the core,
# such as the properties of current TINT version including its checksum.
#
# Copyright (C) 2019 Mike Banon <mikebdp2@gmail.com>
#
################################################################################
#
# USAGE:
#      	./tintify_core.sh <corescript> <tintified> \
# 			<TINT_ARCHIVE_LINK> <TINT_ARCHIVE> <TINT_DIR> <TINT_SHA1SUM>
# where
#       corescript - path to input core script
#       tintified  - path to output tint script
#
################################################################################

corescript="$1"
tintified="$2"

#
# TINT-specific header
#

#
# Insert the properties of the current TINT version
#

echo "#!/bin/sh" > "$tintified"
echo "TINT_ARCHIVE_LINK=${3}" >> "$tintified"
echo "TINT_ARCHIVE=${4}" >> "$tintified"
echo "TINT_DIR=${5}" >> "$tintified"
echo "TINT_SHA1SUM=${6}" >> "$tintified"

#
# Add the replace_plus_with_minus() function - needed to fix the version number
#

echo "replace_plus_with_minus() {" >> "$tintified"
echo "for x in *\"+\"*; do" >> "$tintified"
echo "y=\$(printf %sa \"\$x\" | tr \"+\" \"-\")" >> "$tintified"
echo "mv -- \"\$x\" \"\${y%a}\"" >> "$tintified"
echo "done" >> "$tintified"
echo "}" >> "$tintified"

#
# Add the prepare_TINT() function, it will remove the unneeded debian directory
# as well as typedefs.h and old Makefile to significantly reduce the patch size
#

echo "prepare_TINT() {" >> "$tintified"
# echo "replace_plus_with_minus" >> "$tintified"
echo "if [ ! -z ./\${TINT_DIR} ] && [ -e ./\${TINT_DIR}/debian ] ; then" >> "$tintified"
echo "rm -rf ./\${TINT_DIR}/debian ./\${TINT_DIR}/typedefs.h ./\${TINT_DIR}/Makefile;" >> "$tintified"
echo "touch ./\${TINT_DIR}/Makefile;" >> "$tintified"
echo "fi" >> "$tintified"
echo "}" >> "$tintified"

#
# Importing the core script
#

cat "$corescript" >> "$tintified"

#
# download() function adjustments - became necessary after a version number fix
#

sed -i -e "/download() {/a package=\$1\narchive_link=\"\$(eval echo \\\\\$\$package\"_ARCHIVE_LINK\")\"" "$tintified"
sed -i -e "s/downloading from \$archive/&_link/g" "$tintified"
sed -i -e "s/\(download_showing_percentage \"\$archive\)./\1_link\"/g" "$tintified"

#
# TINT-specific footer
#

echo "if [ ! -d tint ] ; then" >> "$tintified"

echo "printf \"Downloading and verifying TINT tarball ... \\n\"" >> "$tintified"
echo "download TINT || exit \"\$?\"" >> "$tintified"
echo "verify_hash TINT \${TINT_SHA1SUM} || exit \"\$?\"" >> "$tintified"
echo "printf \"Downloaded TINT tarball ... \${green}ok\${NC}\\n\"" >> "$tintified"

echo "printf \"Unpacking and patching TINT... \\n\"" >> "$tintified"
echo "unpack_and_patch TINT || exit 1" >> "$tintified"
echo "printf \"Unpacked and patched TINT... \${green}ok\${NC}\\n\"" >> "$tintified"

echo "mv ./\${TINT_DIR} ./tint" >> "$tintified"
echo "fi" >> "$tintified"

echo "printf \"Building TINT ... \\n\"" >> "$tintified"
echo "make -C ./tint" >> "$tintified"
echo "printf \"TINT built ... \${green}ok\${NC}\\n\"" >> "$tintified"

chmod +x "$tintified"

#
