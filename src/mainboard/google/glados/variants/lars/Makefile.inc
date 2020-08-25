## SPDX-License-Identifier: GPL-2.0-only

romstage-y += variant.c
ramstage-y += variant.c

SPD_SOURCES  = hynix_dimm_H9CCNNN8JTBLAR-NUD-1G-1866    # 0b0000 Single Channel 2GB
SPD_SOURCES += hynix_dimm_H9CCNNNBLTBLAR-NUD-2G-1866    # 0b0001 Dual Channel 8GB
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF-1G-1866     # 0b0010 Dual Channel 4GB
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF-1G-1866     # 0b0011 Single Channel 2GB
SPD_SOURCES += hynix_dimm_H9CCNNNBLTBLAR-NUD-2G-1866    # 0b0100 Single Channel 4GB
SPD_SOURCES += samsung_dimm_K4E6E304EB-EGCF-2G-1866     # 0b0101 Dual Channel 8GB
SPD_SOURCES += hynix_dimm_H9CCNNN8JTBLAR-NUD-1G-1866    # 0b0110 Dual Channel 4GB
SPD_SOURCES += hynix_dimm_H9CCNNNBJTALAR                # 0b0111 Single Channel 4GB
SPD_SOURCES += micron_dimm_MT52L256M32D1PF-107-1G-1866  # 0b1000 Dual Channel 4GB
SPD_SOURCES += micron_dimm_MT52L512M32D2PF-107-2G-1866  # 0b1001 Dual Channel 8GB
SPD_SOURCES += hynix_dimm_H9CCNNN8GTALAR                # 0b1010 Dual Channel 4GB
SPD_SOURCES += micron_dimm_MT52L512M32D2PF-107-2G-1866  # 0b1011 Single Channel 4GB
SPD_SOURCES += samsung_dimm_K4E6E304EB-EGCF-2G-1866     # 0b1100 Single Channel 4GB
SPD_SOURCES += hynix_dimm_H9CCNNNBJTALAR                # 0b1101 Dual Channel 8GB
SPD_SOURCES += empty                                    # 0b1110
SPD_SOURCES += empty                                    # 0b1111
