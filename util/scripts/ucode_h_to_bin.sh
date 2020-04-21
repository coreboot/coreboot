#!/usr/bin/env bash
#
# This file is part of the coreboot project.
#
# Copyright (C) 2015 Google Inc.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

if [ -z "$1" ] || [ -z "$2" ]; then
	printf "Usage: %s <output file> \"<microcode .h files>\"\\n" "$0"
fi

OUTFILE=$1
TMPFILE=$(mktemp microcode_XXXX)
cat > "${TMPFILE}.c" << EOF
#include <stdio.h>
unsigned int microcode[] = {
EOF

include_file() {
	if [ "${1: -4}" == ".inc" ]; then
		sed '/^;/d' <"$1" | awk '{gsub( /h.*$/, "", $2 ); print "0x" $2 ","; }' \
			>> "${TMPFILE}.c"
	else
		echo "#include \"$1\"" >> "${TMPFILE}.c"
	fi
}

for UCODE in ${@:2}; do
	if [ -d "$UCODE" ]; then
		for f in "$UCODE/"*.inc
		do
			include_file "$f"
		done
	else
		include_file "$UCODE"
	fi
done

cat >> "${TMPFILE}.c" << EOF
};
int main(void)
{
	FILE *f = fopen("$OUTFILE", "wb");
	fwrite(microcode, sizeof(microcode), 1, f);
	fclose(f);
	return 0;
}
EOF

gcc -o "$TMPFILE" "${TMPFILE}.c"
[ -f "${TMPFILE}.exe" ] && mv "${TMPFILE}.exe" "$TMPFILE"
"./$TMPFILE"
rm "$TMPFILE" "${TMPFILE}.c"
