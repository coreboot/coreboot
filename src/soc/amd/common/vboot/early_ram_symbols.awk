#!/usr/bin/awk -F
#
# SPDX-License-Identifier: GPL-2.0-only
#
# Generates a linker script to provide early RAM symbols.
#
# e.g.,
# _cbmemc_transfer = 0x02014040;
# _ecbmemc_transfer = 0x02015640;
# _cbmemc_transfer_size = 0x00001600;

$3 ~ /^_e?transfer_buffer$/ { printf("%s = 0x%s;\n", $3, $1)}
$3 ~ /^_transfer_buffer_size$/ { printf("%s = 0x%s;\n", $3, $1)}

$3 ~ /^_e?cbmemc_transfer$/ { printf("%s = 0x%s;\n", $3, $1)}
$3 ~ /^_cbmemc_transfer_size$/ { printf("%s = 0x%s;\n", $3, $1)}
