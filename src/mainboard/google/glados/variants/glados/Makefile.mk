## SPDX-License-Identifier: GPL-2.0-only

romstage-y += variant.c
ramstage-y += variant.c
smm-y += variant.c

# SPD data by index.  No method for board identification yet
SPD_SOURCES = empty                             # 0b0000
SPD_SOURCES += samsung_dimm_K4E6E304EE-EGCF     # 0b0001
SPD_SOURCES += hynix_dimm_H9CCNNN8JTBLAR        # 0b0010
SPD_SOURCES += hynix_dimm_H9CCNNNBLTALAR        # 0b0011
