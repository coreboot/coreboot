#!/bin/sh
#
# This file is part of the coreboot project. It originated in the
# flashrom project but has been heavily modified since then.
#
# Copyright (C) 2013 Stefan Tauner
# Copyright (C) 2013 Google Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc.
#

EXIT_SUCCESS=0
EXIT_FAILURE=1

# Make sure we don't get translated output
export LC_ALL=C
# nor local times or dates
export TZ=UTC0

# Helper functions
git_has_local_changes() {
	git update-index -q --refresh >/dev/null
	! git diff-index --quiet HEAD -- "$1"
}

git_last_commit() {
	git log --pretty=format:"%h" -1 -- "$1"
}

git_is_file_tracked() {
	git ls-files --error-unmatch -- "$1" >/dev/null 2>&1
}

is_file_tracked() {
	git_is_file_tracked "$1"
}

# Tries to find a remote source for the changes committed locally.
# This includes the URL of the remote repository including the last commit and a suitable branch name.
# Takes one optional argument: the path to inspect
git_url() {
  # Note: This may not work as expected if multiple remotes are fetched from.
  echo $(git remote -v | grep "^origin\>" | \
         awk '/fetch/ {print $2; exit 0}' | sed "s,^.*@,,")
}

# Returns a string indicating where others can get the current source code (excluding uncommitted changes)
# Takes one optional argument: the path to inspect
scm_url() {
	local url

	url="$(git_url "$1")"

	echo "${url}"
}

# Retrieve timestamp since last modification. If the sources are pristine,
# then the timestamp will match that of the SCM's most recent modification
# date.
timestamp() {
	local t

	# date syntaxes are manifold:
	# gnu		date [-d input]... [+FORMAT]
	# netbsd	date [-ajnu] [-d date] [-r seconds] [+format] [[[[[[CC]yy]mm]dd]HH]MM[.SS]]
	# freebsd	date [-jnu]  [-d dst] [-r seconds] [-f fmt date | [[[[[cc]yy]mm]dd]HH]MM[.ss]] [+format] [...]
	# dragonflybsd	date [-jnu]  [-d dst] [-r seconds] [-f fmt date | [[[[[cc]yy]mm]dd]HH]MM[.ss]] [+format] [...]
	# openbsd	date [-aju]  [-d dst] [-r seconds] [+format] [[[[[[cc]yy]mm]dd]HH]MM[.SS]] [...]
	if git_is_file_tracked "$2" ; then
		# are there local changes?
		if git_has_local_changes "$2" ; then
			t=$(date -u "${1}")
		else
			# No local changes, get date of the last commit
			case $(uname) in
			# Most BSD dates do not support parsing date values from user input with -d but all of
			# them support parsing epoch seconds with -r. Thanks to git we can easily use that:
			NetBSD|OpenBSD|DragonFly|FreeBSD)
				t=$(date -u -r "$(git log --pretty=format:%ct -1 -- $2)"  "$1" 2>/dev/null);;
			*)
				t=$(date -d "$(git log --pretty=format:%cD -1 -- $2)" -u "$1" 2>/dev/null);;
			esac
		fi
	else
		t=$(date -u "$1")
	fi

	if [ -z "$t" ]; then
		echo "Warning: Could not determine timestamp." 2>/dev/null
	fi
	echo "${t}"
}

# Retrieve local SCM revision info. This is useful if we're working in a different SCM than upstream and/or
# have local changes.
local_revision() {
	local r

	if git_is_file_tracked "$1" ; then
		r=$(git_last_commit "$1")

		if git_has_local_changes "$1" ; then
			r="$r-dirty"
		fi
	else
		return ${EXIT_FAILURE}
	fi

	echo "${r}"
}

# Similar to local_revision but uses "git describe" instead of "git log" which
# includes number of commits since most recent tag.
tagged_revision() {
	local r

	if git_is_file_tracked "$1" ; then
		r=$(git describe --tags --dirty)
	else
		return ${EXIT_FAILURE}
	fi

	echo "${r}"
}

upstream_revision() {
	local r=

	r=$(git log remotes/origin/master -1 --format=format:%h)

	if [ -z "$r" ]; then
		r="unknown" # default to unknown
	fi
	echo "${r}"
}

show_help() {
	echo "Usage:
	${0} <command> [path]

Commands
    -h or --help
        this message
    -l or --local
        local revision information including an indicator for uncommitted changes
    -u or --upstream
        upstream revision
    -T or --tags
        similar to -l, but uses \"git describe\" to obtain revision info with tags
    -U or --url
        URL associated with the latest commit
    -d or --date
        date of most recent modification
    -t or --timestamp
        timestamp of most recent modification
"
	return
}

check_action() {
	if [ -n "$action" ]; then
		echo "Error: Multiple actions given.">&2
		exit ${EXIT_FAILURE}
	fi
}

main() {
	local query_path=
	local action=

	# The is the main loop
	while [ $# -gt 0 ];
	do
		case ${1} in
		-h|--help)
			action=show_help;
			shift;;
		-l|--local)
			check_action $1
			action=local_revision
			shift;;
		-T|--tags)
			check_action $1
			action=tagged_revision
			shift;;
		-u|--upstream)
			check_action $1
			action=upstream_revision
			shift;;
		-U|--url)
			check_action $1
			action=scm_url
			shift;;
		-d|--date)
			check_action $1
			action="timestamp +%Y-%m-%d" # refrain from suffixing 'Z' to indicate it's UTC
			shift;;
		-t|--timestamp)
			check_action $1
			action="timestamp +%Y-%m-%dT%H:%M:%SZ" # There is only one valid time format! ISO 8601
			shift;;
		-*)
			show_help;
			echo "Error: Invalid option: ${1}"
			exit ${EXIT_FAILURE};;
		*)
			if [ -z "$query_path" ] ; then
				if [ ! -e "$1" ] ; then
					echo "Error: Path \"${1}\" does not exist.">&2
					exit ${EXIT_FAILURE}
				fi
				query_path=$1
			else
				echo "Warning: Ignoring over-abundant paramter: \"${1}\"">&2
			fi
			shift;;
		esac;
	done

	# default to current directory (usually equals the whole repository)
	if [ -z "$query_path" ] ; then
		query_path=.
	fi
	if ! is_file_tracked "$query_path" ; then
		echo "Warning: Path \"${query_path}\" is not under version control.">&2
	fi
	if [ -z "$action" ] ; then
		show_help
		echo "Error: No actions specified"
		exit ${EXIT_FAILURE}
	fi

	$action "$query_path"
}

main $@
