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

# static analysis
if command -v shellcheck 1>/dev/null; then
	shellcheck --exclude=1090,1091 \
		"${BASH_SOURCE[0]}" \
		"$(dirname "${BASH_SOURCE[0]}")/helpers.sh"
else
	echo "shellcheck not found, running unchecked" >&2
fi

# dependency check
dependencies=(dirname git make mktemp rm timeout)
for dependency in "${dependencies[@]}"; do
	if ! command -v "${dependency}" 1>/dev/null; then
		echo "missing ${dependency}, test skipped" >&2
		exit 0
	fi
done

source "$(dirname "${BASH_SOURCE[0]}")/helpers.sh"

# setup
base_dir="$(mktemp --directory --tmpdir \
	"test-$(basename "${BASH_SOURCE[0]}" .sh)-XXXXXXXX")"
clone_dir="${base_dir}/coreboot"
git clone "$(git rev-parse --show-toplevel)" "${clone_dir}" \
	1>"${base_dir}/clone.log" 2>&1

(
	set -o errexit
	set -o nounset

	clone_submodules "${clone_dir}" "${base_dir}"

	# mock
	git config user.name "John Doe"
	git config user.email "john.doe@example.com"

	# test
	log_file="${base_dir}/gitconfig.log"
	timeout 2s make gitconfig \
		1>"${log_file}" 2>&1 \
		|| check_exit_code positive "${log_file}"
)

# teardown
rm --force --recursive "${base_dir}"
