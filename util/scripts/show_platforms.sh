#!/bin/bash
#
# SPDX-License-Identifier: GPL-2.0-only
#
# This script finds all of the top-level mainboards, then goes through
# and finds the date the directory was added, the processor type, and
# the board type.
#
# This could be improved by finding all of the variants, then figuring
# out when those veriants were added.
# It's also very slow, but only needs to be run once in a while...

readarray -t platforms < <(find src/mainboard -mindepth 3 -name 'Kconfig' | sort)

echo '```eval_rst'
echo "+-------------------------------+------------------------+------------+-----------+"
echo "| Vendor/Board                  | Processor              | Date added | Brd type  |"
echo "+===============================+========================+============+===========+"

for file in "${platforms[@]}"; do
  platformname="$(echo "${file}" | sed 's|.*/mainboard/||;s|/Kconfig||')"
  if [[ ! -f "${file/Kconfig/board_info.txt}" ]]; then
    continue
  fi
  chips="$(grep "CPU_\|SOC_\|NORTHBRIDGE" "${file}" |
    grep -v "SUBTYPE\|COMMON\|SOCKET\|ENABLE\|CONSOLE\|SMU\|depends on\|ESPI\|INTEL_CSE\|NORTHBRIDGE_AMD_AGESA\|INTEL_SLOT\|REBOOT\|DISABLE" |
    sed -e 's|\s\+select\s\+||' \
      -e 's|\s\+if.*||' \
      -e 's|SKYLAKE_SOC_PCH|INTEL_SKYLAKE|' \
      -e 's|CPU_AMD_AGESA|AMD|' \
      -e 's|SOC_INTEL_ALDERLAKE_PCH_|INTEL_ALDERLAKE|' \
      -e 's|QC_|QUALCOMM_|' \
      -e 's/SOC_\|NORTHBRIDGE_\|PCH_\|CPU_//g' |
    sort -u)"
  if [[ ! -f ${file/Kconfig/board_info.txt} ]]; then
    continue
  fi
  create_date="$(git log --format="format:%cs" -- "${file}" | tail -n1)"
  platform_type="$(sed -nE -e 's/Category: (.*)/\1/p' "${file/Kconfig/board_info.txt}" 2>/dev/null)"
  for chip in ${chips}; do

    printf "| %-29s | %-22s | %-10s | %-9s |\n" "${platformname}" "${chip}" "${create_date}" "${platform_type}"
  done
done

echo "+-------------------------------+------------------------+------------+-----------+"
echo '```'
