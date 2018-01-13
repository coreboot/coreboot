#!/bin/bash

##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 secunet Security Networks AG
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

if ! { cdup="$(git rev-parse --show-cdup 2>/dev/null)" && [ -z "${cdup}" ]; }
then
	echo "Error: Not in root of a git repository"
	exit 1
fi
coreboot_hooks=$(git rev-parse --git-path hooks)
mkdir -p "${coreboot_hooks}"
for hook in commit-msg pre-commit ; do
	if [ util/gitconfig/${hook} -nt "${coreboot_hooks}/${hook}" ] \
		|| [ ! -x "${coreboot_hooks}/${hook}" ]
	then
		sed -e "s,%MAKE%,remake,g" util/gitconfig/$hook \
			> "${coreboot_hooks}/${hook}"
		chmod +x "${coreboot_hooks}/${hook}"
	fi
done
# Now set up the hooks for 3rdparty/
for submodule in 3rdparty/blobs libhwbase libgfxinit; do
	hooks=$(git -C "$(git config --file .gitmodules --get \
		submodule.${submodule}.path)" rev-parse --git-path hooks)
	if [ -d "${hooks}" ]; then
		if [ util/gitconfig/commit-msg -nt "${hooks}/commit-msg" ] \
			|| [ ! -x "${hooks}/commit-msg" ]
		then
			sed -e "s,%MAKE%,remake,g" util/gitconfig/commit-msg \
				> "${hooks}/commit-msg"
			chmod +x "${hooks}/commit-msg"
		fi
	fi
done
for d in 3rdparty/{blobs,libhwbase,libgfxinit}; do
	if [ -d $d ]; then
		(cd $d || exit 1
		 git config remote.origin.push HEAD:refs/for/master)
	fi
done

git config remote.origin.push HEAD:refs/for/master
git config alias.sup "!git submodule update --remote --rebase && \
git submodule update --init --checkout"

git config alias.sup-destroy "!git submodule deinit --all --force; \
git submodule update --init --checkout"

{ git config --includes user.name && \
	git config --includes user.email; } >/dev/null || \
	{ printf "Please configure your name and email in git:\n\n\
 git config --global user.name \"Your Name Comes Here\"\n\
 git config --global user.email your.email@example.com\n"; \
exit 1; }
