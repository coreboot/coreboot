#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause

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
