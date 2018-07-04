#!/bin/sh
# This file is part of the coreboot project.
#
# Copyright (C) 2018 Google Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
set -e

# This script is the entry point for this container and expects two git
# repositories in /data-in, board-status.git and coreboot.git (with the
# content of the repos of the same name found at https://review.coreboot.org/
# and creates two files, board-status.html and kconfig-options.html in
# /data-out.

cd /data-in/board-status.git
/opt/tools/status-to-html.sh > /tmp/board-status.html
mv /tmp/board-status.html /data-out/

cd /data-in/coreboot.git
/opt/tools/kconfig2html src/Kconfig $(git describe) > /tmp/kconfig-options.html
mv /tmp/kconfig-options.html /data-out/
