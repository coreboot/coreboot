#!/bin/sh

# rebase.sh - rebase helper script
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

# Adapt to your remote branch:
BRANCH="cros/chromeos-2015.07"

# When pulling in patches from another tree from a gerrit repository,
# do the following at the end of a larger cherry-pick series:
#  git remote add ...
#  git checkout -b upstreaming
#  git cherry-pick ...
#  git rebase -i --exec util/gitconfig/rebase.sh master
# Alternatively, you can run util/gitconfig/rebase.sh after every
# individual cherry-pick.

commit_message() {
	git log -n 1 | grep "^    " | cut -c5-
}

CHID=$( commit_message | grep -i "^Change-Id: I" )
CID=$( git log -n1 --grep "^$CHID$" --pretty=%H $BRANCH )
GUID="$(git config user.name) <$(git config user.email)>"

# TBD: Don't add Original- to empty lines, and possibly make script more
# solid for commits with an unexpected order of meta data lines.

commit_message | tac | awk '/^$/ {
		if (end==0)
			print "Original-Commit-Id: '"$CID"'\nSigned-off-by: '"$GUID"'";
		end=1
	}; {
		if (end==0)
			print "Original-" $0;
		else
			print $0;
	}' | tac | git commit --amend -F -
