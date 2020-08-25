## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd_util.c

SPD_SOURCES  = rvp3				# 0b000 Dual Channel 4GB
SPD_SOURCES += empty				# 0b001
SPD_SOURCES += empty				# 0b010
SPD_SOURCES += empty				# 0b011
SPD_SOURCES += empty				# 0b100
SPD_SOURCES += empty				# 0b101
SPD_SOURCES += hynix_dimm_H9CCNNNBJTMLAR	# 0b110 Dual Channel 8GB
SPD_SOURCES += empty				# 0b111
