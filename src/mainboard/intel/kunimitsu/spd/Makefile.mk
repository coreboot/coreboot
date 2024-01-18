## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd_util.c

SPD_SOURCES  = hynix_dimm_H9CCNNN8JTALAR-NUD-1G-1866    # 0b0000 Dual Channel 4GB
SPD_SOURCES += hynix_dimm_H9CCNNN8JTALAR-NUD-1G-1866    # 0b0001 Single Channel 2GB
SPD_SOURCES += samsung_dimm_K4E8E304EE-EGCF-1G-1866     # 0b0010 Dual Channel 4GB
SPD_SOURCES += samsung_dimm_K4E6E304EE-EGCF-2G-1866     # 0b0011 Dual Channel 8GB
SPD_SOURCES += samsung_dimm_K4E8E304EE-EGCF-1G-1866     # 0b0100 Single Channel 4GB
SPD_SOURCES += mic_dimm_EDF8132A3MA-JD-F-1G-1866        # 0b0101 Single Channel 4GB SPD FILE NOT HERE YET
SPD_SOURCES += hynix_dimm_H9CCNNN8JTBLAR-NUD-1G-1866    # 0b0110 Dual Channel 4GB SPD FILE NOT HERE YET
SPD_SOURCES += hynix_dimm_H9CCNNNBLTBLAR-NUD-2G-1866    # 0b0111 Dual Channel 8GB SPD FILE NOT HERE YET
SPD_SOURCES += empty                                    # 0b1000
SPD_SOURCES += empty                                    # 0b1001
SPD_SOURCES += empty                                    # 0b1010
SPD_SOURCES += empty                                    # 0b1011
SPD_SOURCES += empty                                    # 0b1100
SPD_SOURCES += empty                                    # 0b1101
SPD_SOURCES += empty                                    # 0b1110
SPD_SOURCES += empty                                    # 0b1111
