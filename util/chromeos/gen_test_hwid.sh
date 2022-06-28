#!/usr/bin/env sh
#
# SPDX-License-Identifier: GPL-2.0-only

main() {
  if [ "$#" != 1 ]; then
    echo "Usage: $0 MAINBOARD_PARTNUMBER" >&2
    exit 1
  fi

  # Generate a test-only ChromeOS HWID v2 string
  local board="$1"
  local prefix="$(echo "${board}" | tr a-z A-Z) TEST"
  # gzip has second-to-last 4 bytes in CRC32.
  local crc32="$(printf "${prefix}" | gzip -1 | tail -c 8 | head -c 4 | \
		 hexdump -e '1/4 "%04u" ""' | tail -c 4)"

  echo "${prefix}" "${crc32}"
}
main "$@"
