## SPDX-License-Identifier: GPL-2.0-only

romstage-y += variant.c
ramstage-y += variant.c

SPD_SOURCES  = hynix_dimm_H9CCNNN8GTMLAR-NUD		# 0b0000 Dual Channel 4GB
SPD_SOURCES += micron_dimm_MT52L256M32D1PF-107-1G-1866	# 0b0001 Dual Channel 4GB
SPD_SOURCES += samsung_dimm_K4E8E304EE-EGCF		# 0b0010 Dual Channel 4GB
SPD_SOURCES += samsung_dimm_K4E6E304EB-EGCF		# 0b0011 Dual Channel 8GB
SPD_SOURCES += micron_dimm_MT52L512M32D2PF-107-2G-1866	# 0b0100 Dual Channel 8GB
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF		# 0b0101 Dual Channel 8GB
SPD_SOURCES += hynix_dimm_H9CCNNNBJTMLAR-NUD		# 0b0110 Dual Channel 4GB
SPD_SOURCES += hynix_dimm_H9CCNNNBLTBLAR-NUD-2G-1866	# 0b0111 Dual Channel 8GB
SPD_SOURCES += empty					# 0b1000
SPD_SOURCES += empty					# 0b1001
SPD_SOURCES += empty					# 0b1010
SPD_SOURCES += empty					# 0b1011
SPD_SOURCES += empty					# 0b1100
SPD_SOURCES += empty					# 0b1101
SPD_SOURCES += empty					# 0b1110
SPD_SOURCES += empty					# 0b1111
