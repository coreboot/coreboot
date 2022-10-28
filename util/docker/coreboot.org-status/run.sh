#!/usr/bin/env sh
#
# SPDX-License-Identifier: GPL-2.0-only

set -e

# This script is the entry point for this container and expects two git
# repositories in /data-in, board-status.git and coreboot.git (with the
# content of the repos of the same name found at https://review.coreboot.org/
# and creates two files, board-status.html and kconfig-options.html in
# /data-out.

cd /data-in/board-status.git
/opt/tools/board-status.html \
	-board-status-dir /data-in/board-status.git \
	-coreboot-dir /data-in/coreboot.git \
	> /tmp/board-status.html
mv /tmp/board-status.html /data-out/

cd /data-in/coreboot.git
/opt/tools/kconfig2html src/Kconfig $(git describe) > /tmp/kconfig-options.html
mv /tmp/kconfig-options.html /data-out/
