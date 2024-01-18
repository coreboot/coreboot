## SPDX-License-Identifier: GPL-2.0-only

romstage-y += variant.c
ramstage-y += variant.c

SPD_SOURCES  = hynix_dimm_H9CCNNN8GTMLAR-NUD            # 0b0000 Single Channel 2GB
SPD_SOURCES += hynix_dimm_H9CCNNN8GTMLAR-NUD            # 0b0001 Dual Channel 4GB
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF             # 0b0010 Single Channel 2GB
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF             # 0b0011 Dual Channel 4GB
SPD_SOURCES += micron_dimm_MT52L256M32D1PF-107-1G-1866  # 0b0100 Single Channel 2GB
SPD_SOURCES += micron_dimm_MT52L256M32D1PF-107-1G-1866  # 0b0101 Dual Channel 4GB
