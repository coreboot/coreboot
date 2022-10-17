#!/usr/bin/env sh
#
# SPDX-License-Identifier: GPL-2.0-only

# This file is sourced by the linters so that each one doesn't have to
# specify these routines individually

LC_ALL=C export LC_ALL

if [ -z "$GIT" ]; then
	GIT="$(command -v git)"
else
	# If git is specified, Do a basic check that it runs and seems like
	# it's actually git
	if ! "${GIT}" --version | grep -q git; then
		echo "Error: ${GIT} does not seem to be valid."
		exit 1;
	fi
fi

if [ "$(${GIT} rev-parse --is-inside-work-tree 2>/dev/null)" = "true" ]; then
	IN_GIT_TREE=1
else
	IN_GIT_TREE=0
fi

if [ "${IN_GIT_TREE}" -eq 1 ] && [ -z "${GIT}" ]; then
	echo "This test needs git to run.  Please install it, then run this test again."
	exit 1
fi

# Use git ls-files if the code is in a git repo, otherwise use find.
if [ "${IN_GIT_TREE}" -eq 1 ]; then
	FIND_FILES="${GIT} ls-files"
else
	FIND_FILES="find "
	FINDOPTS="-type f"
fi

# Use git grep if the code is in a git repo, otherwise use grep.
if [ "${IN_GIT_TREE}" -eq 1 ]; then
	GREP_FILES="${GIT} grep"
else
	GREP_FILES="grep -r"
fi
