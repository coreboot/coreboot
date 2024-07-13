#!/usr/bin/env sh
## SPDX-License-Identifier: BSD-3-Clause

# This script allows us to capture all the commands run in the shell by the
# coreboot build. This is better than using 'make v=1' because it captures
# commands right from the beginning of the build, and sends the output of any
# command to the interactive shell.

# To use, run:
# make SHELL="${PWD}/util/scripts/capture_commands.sh"

REALSHELL=${REALSHELL:-sh}
OUTPUT_DIR=${TOP:-/tmp}
CAPTURE_FILE=${CAPTURE_FILE:-commands.txt}

env echo "$*" | sed 's/^-c '// >> "${OUTPUT_DIR}/${CAPTURE_FILE}"
"${REALSHELL}" "$@"
