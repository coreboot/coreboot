#!/usr/bin/env bash
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 3 or later of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## SPDX-License-Identifier: GPL-3.0-or-later
## <https://spdx.org/licenses/GPL-3.0-or-later.html>
##

set -o errexit
set -o nounset

# dependency check
dependencies=(cut git readlink)
for dependency in "${dependencies[@]}"; do
	if ! command -v "${dependency}" 1>/dev/null; then
		echo "missing ${dependency}, test skipped" >&2
		exit 0
	fi
done

# helper functions
function clone_submodules() {
	clone_dir="${1}"
	log_dir="${2}"

	modules_dir="$(readlink --canonicalize-missing \
		"$(git rev-parse --git-dir)/modules")"
	cd "${clone_dir}"
	git submodule init 1>>"${log_dir}/clone.log" 2>&1
	for submodule in $(git config --get-regexp "submodule\..*\.url" \
		| cut --delimiter=. --fields=2); do
		git config "submodule.${submodule}.url" \
			"${modules_dir}/${submodule}"
	done
	git submodule update 1>>"${log_dir}/clone.log" 2>&1
}

function check_exit_code() {
	declare -i err=${?}

	# either "positive" or "negative"
	polarity="${1}"
	log_file="${2}"

	# exit code 124 is special as per `timeout` manpage
	if [ "${polarity}" == "positive" ] && [ ${err} -eq 124 ]; then
		echo >&2 "timed out"
	fi

	if [ "${polarity}" == "positive" ] && [ ${err} -ne 0 ]; then
		echo "bad exit code: expected 0, actually ${err}"
		echo "for details, refer to log file \"${log_file}\""
		exit ${err}
	elif [ "${polarity}" == "negative" ] && [ ${err} -eq 0 ]; then
		echo "bad exit code: expected non-zero, actually 0"
		echo "for details, refer to log file \"${log_file}\""
		exit 1
	fi
}
