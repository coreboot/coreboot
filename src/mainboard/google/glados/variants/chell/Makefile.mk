## SPDX-License-Identifier: GPL-2.0-only

romstage-y += variant.c
ramstage-y += variant.c
smm-y += variant.c

# SPD data by index.  No method for board identification yet
SPD_SOURCES = samsung_dimm_K4E8E304EE-EGCF      # 0b0000
SPD_SOURCES += samsung_dimm_K4E6E304EE-EGCF     # 0b0001
SPD_SOURCES += hynix_dimm_H9CCNNN8GTMLAR        # 0b0010
SPD_SOURCES += hynix_dimm_H9CCNNNBJTMLAR        # 0b0011
SPD_SOURCES += hynix_dimm_H9CCNNNCLTMLAR        # 0b0100
SPD_SOURCES += samsung_dimm_K4E6E304EB-EGCF     # 0b0101
SPD_SOURCES += samsung_dimm_K4EBE304EB-EGCF     # 0b0110
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF     # 0b0111
SPD_SOURCES += hynix_dimm_H9CCNNNBJTALAR        # 0b1000
SPD_SOURCES += hynix_dimm_H9CCNNNCLGALAR        # 0b1001
SPD_SOURCES += hynix_dimm_H9CCNNN8GTALAR        # 0b1010
