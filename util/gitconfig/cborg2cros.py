#!/usr/bin/env python

# This file is part of the coreboot project.
#
# Copyright (C) 2016 Google, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

import sys
import re
import subprocess

# Regular expression patterns
pat_change_id = re.compile('^Change-Id: (.*)$')
pat_orig_lines = re.compile('^(Signed-off-by|Reviewed-on|Reviewed-by)')
pat_uninteresting = re.compile('^Tested-by')
pat_quotes = re.compile('"')
pat_leading_space = re.compile('^ ')
pat_bug_line = re.compile('^bug\s*=', re.IGNORECASE)
pat_branch_line = re.compile('branch\s*=', re.IGNORECASE)
pat_test_line = re.compile('test\s*=', re.IGNORECASE)

def main():
    branch = ""
    new_commit_message = ""
    change_id = ""
    commit_id = ""
    bug_line = "BUG=None\n"
    branch_line = "BRANCH=None\n"
    test_line = "TEST=Build tested at coreboot.org\n"

    # Check command line arguments
    if len(sys.argv) > 1:
        if sys.argv[1] == "-h" or sys.argv[1] == "--help":
            print "Update the commit message to submit to the Chrome OS tree."
            print "Usage: " + sys.argv[1] + " [Remote branch]\n"
        else:
            branch = sys.argv[1]
    else:
        branch = "cborg/master"

    # Get the last commit message, then loop through looking at each line
    commit_message = subprocess.check_output(["git", "log", "-n1"]).split("\n")
    for line in commit_message:

        # Skip the initial few lines of the commit message
        m = pat_leading_space.match(line)
        if not m:
            continue

        # Remove initial whitespace
        line = line.lstrip(' ')

        # Add the 'UPSTREAM' comment to the subject line
        if len(new_commit_message) == 0:
            new_commit_message += "UPSTREAM: " + line + "\n"
            continue

        # If we've found a TEST, BRANCH, or BUG line, mark it as found
        if pat_test_line.match(line):
            test_line = ""
        if pat_bug_line.match(line):
            bug_line = ""
        if pat_branch_line.match(line):
            branch_line = ""

        # Grab the Change-Id
        chid = pat_change_id.match(line)
        if chid:
            change_id = chid.group(1)

        # Add 'Original-' to all of the coreboot.org gerrit messages
        grrt = pat_orig_lines.match(line)
        if grrt:
            line = "Original-" + line

        # if we've reached the end of the real commit message text and we don't
        # have the required TEST= BUG= and BRANCH= lines, add them.
        if (chid or grrt) and (bug_line or branch_line or test_line):
            new_commit_message += bug_line + branch_line + test_line + "\n"
            bug_line = branch_line = test_line = ""

        # Remove uninteresting gerrit messages
        if pat_uninteresting.match(line):
            continue

        # Add the current line to the updated commit message
        new_commit_message += line + "\n"

    # Error out if no Change-Id was located
    if not change_id:
        print "Error: No Change-Id found"
        sys.exit(1)

    # Get the Commit ID based on the Change-Id and the branch.
    # Error out if git returns an error
    try:
        commit_id = subprocess.check_output(["git", "log", "-n1", "--grep", change_id, '--pretty=%H', branch, "--"]).rstrip('\n')
    except:
        print "Error: invalid branch - " + branch + ".\n"
        sys.exit(1)

    # To find the Commit-Id, we've looked through the git log for a particular Change-Id
    # Error out if the Commit-Id wasn't found, with the message that we couldn't find the Change-Id
    if not commit_id:
        print "Error: Could not find Change-Id: " + change_id + " in branch " + branch + ".\n"
        sys.exit(1)

    # Add the Commit-Id that this change came from to the new commit message.
    new_commit_message += "(cherry-picked from commit " + commit_id + ")\n"

    # Update the commit message
    amend_output = subprocess.check_output(["git", "commit", "-s", "--amend", "-m", new_commit_message ])

    print "----------\n"
    print amend_output

if __name__ == "__main__":
    main()
