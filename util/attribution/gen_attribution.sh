#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Render the coreboot third-party attribution / notices file.
#
# Usage:
#   gen_attribution.sh [--extra <file>] <manifest> <key> [<key> ...]
#
# --extra     optional file of pre-resolved rows to emit unconditionally, in
#             the same pipe-delimited format as the manifest. Used to inject
#             build-time-only information (e.g. the actual configured URL/revision
#             of a payload repository) that a static manifest cannot express.
#             Its 'key' column is ignored (rows are not key-gated).
# <manifest>  path to attribution.list
# <key> ...   keys of the projects that are included in this build; these are
#             selected by util/attribution/Makefile.mk from the enabled Kconfig
#             options. Rows are emitted in manifest order, not argument order,
#             so the output is stable regardless of how make orders the keys.
#
# Entries render under one of three headings depending on their 'kind' field:
# software compiled into the image (empty), borrowed code (used), and binaries
# the build stitches in (binary).
#
# The rendered document is written to stdout.

set -eu

usage() {
	echo "usage: $0 [--extra <file>] <manifest> <key>..." >&2
	exit 1
}

extra=""
if [ "${1:-}" = "--extra" ]; then
	# Guard against '--extra' being the final argument: consuming $2 under
	# 'set -u' would otherwise die with a cryptic unbound-variable error.
	[ "$#" -ge 2 ] || usage
	extra=$2
	shift 2
fi

if [ "$#" -lt 2 ]; then
	usage
fi

manifest=$1
shift

if [ ! -r "${manifest}" ]; then
	echo "$0: cannot read manifest '${manifest}'" >&2
	exit 1
fi

# Build a lookup set of enabled keys as " key1 key2 ... " for case matching.
enabled=" "
for key in "$@"; do
	enabled="${enabled}${key} "
done

# Emit one block per matching row of a file.
#   $1 file, $2 wanted kind (bundled|used|binary), $3 gate (1 = key-gated)
# Fields are pipe-delimited: key|name|license|url|copyright|kind
emit_rows() {
	_file=$1
	_want=$2
	_gate=$3
	[ -n "${_file}" ] && [ -r "${_file}" ] || return 0
	# '|| [ -n "${key}" ]' processes a final line lacking a trailing newline,
	# which 'read' would otherwise return non-zero on and silently drop.
	while IFS='|' read -r key name license url copyright kind || [ -n "${key}" ]; do
		case "${key}" in
		''|'#'*) continue ;;	# skip blank lines and comments
		esac
		if [ "$_gate" = 1 ]; then
			case "${enabled}" in
			*" ${key} "*) ;;		# enabled
			*) continue ;;
			esac
		fi
		case "${_want}" in
		used)    [ "${kind}" = used ] || continue ;;
		binary)  [ "${kind}" = binary ] || continue ;;
		bundled) case "${kind}" in used|binary) continue ;; esac ;;
		esac

		printf '\n* %s\n' "${name}"
		printf '    License:  %s\n' "${license}"
		printf '    Upstream: %s\n' "${url}"
		if [ -n "${copyright}" ]; then
			printf '    %s\n' "${copyright}"
		fi
	done < "${_file}"
}

# A section is the key-gated manifest rows followed by the ungated extra rows.
emit_section() {
	emit_rows "${manifest}" "$1" 1
	emit_rows "${extra}" "$1" 0
}

printf 'coreboot third-party attribution / notices\n'
printf 'Note that additional files may be added to the ROM image that are not built\n'
printf 'from the coreboot source tree.\n\n'

printf 'This coreboot components includes software from the following projects:\n'
emit_section bundled

# Only print the "Code used from" heading if any such entry is enabled.
used_out=$(emit_section used)
if [ -n "${used_out}" ]; then
	printf '\nCode from the following projects has been used in coreboot:\n'
	printf '%s\n' "${used_out}"
fi

# Standalone binaries the build adds to the image but does not compile from
# coreboot source (payloads, vendor firmware blobs, separately-built programs).
bin_out=$(emit_section binary)
if [ -n "${bin_out}" ]; then
	printf '\nThe following payloads and binaries are added to the image,\n'
	printf 'but are not compiled directly against coreboot source:\n'
	printf '%s\n' "${bin_out}"
fi

printf '\nFull license texts are provided with the coreboot source under LICENSES/.\n'
