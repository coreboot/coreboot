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
	git config user.name "John Doe"
	git config user.email "john.doe@example.com"
	make gitconfig

	# test
	echo "good case..."
	log_file="${base_dir}/good_case.log"
	echo "this is a test" >> README.md
	timeout 4m git commit --all --signoff --message="good case" \
		1>"${log_file}" 2>&1 \
		|| check_exit_code positive "${log_file}"
	git reset --hard --quiet HEAD^
	git clean -d --force --quiet -x

	echo "bad case..."
	log_file="${base_dir}/bad_case.log"
	# Goal here is to verify whether a failing `util/lint` test will prevent
	# a commit. It's a bit tricky because `checkpatch.pl` is run just after
	# the lint tests and will cover many of those too. So we need a case
	# that fails with `util/lint` but succeeds with `checkpatch.pl`. I found
	# that `lint-stable-009-old-licenses` does the job quite well.
	printf "You should have received a copy of the %s\n" "GNU" > src/test.c
	git add src/test.c
	timeout 4m git commit --signoff --message="bad case" \
		1>"${log_file}" 2>&1 \
		&& check_exit_code negative "${log_file}"
	git rm --force --quiet src/test.c
	git clean -d --force --quiet -x
)

# teardown
rm --force --recursive "${base_dir}"
