## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd.c

# Order matters for SPD sources. The following indices
# define the SPD data to use.
SPD_SOURCES = micron_4GiB_dimm_MTA9ASF51272PZ-2G1A2

spd.bin-position := $(CONFIG_SPD_LOC)
