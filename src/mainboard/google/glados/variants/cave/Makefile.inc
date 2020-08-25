## SPDX-License-Identifier: GPL-2.0-only

romstage-y += variant.c
ramstage-y += variant.c
smm-y += variant.c

# SPD data by index.  No method for board identification yet
SPD_SOURCES = micron_4GiB_dimm_MT52L256M32D1PF  # 0b0000
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF     # 0b0001
SPD_SOURCES += micron_8GiB_dimm_MT52L512M32D2PF # 0b0010
SPD_SOURCES += samsung_dimm_K4E6E304EB-EGCF     # 0b0011
SPD_SOURCES += micron_16GiB_dimm_MT52L1G32D4PG  # 0b0100
SPD_SOURCES += hynix_dimm_H9CCNNNCLTMLAR        # 0b0101
