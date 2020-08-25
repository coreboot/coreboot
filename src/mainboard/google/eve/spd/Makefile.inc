## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd.c

SPD_SOURCES = empty				# 0b0000
SPD_SOURCES += samsung_dimm_K4E8E324EB		# 0b0001
SPD_SOURCES += hynix_dimm_H9CCNNN8GTALAR	# 0b0010
SPD_SOURCES += samsung_dimm_K4E6E304EB		# 0b0011
SPD_SOURCES += hynix_dimm_H9CCNNNBJTALAR	# 0b0100
SPD_SOURCES += samsung_dimm_K4EBE304EB		# 0b0101
SPD_SOURCES += hynix_dimm_H9CCNNNCLTMLAR	# 0b0110
