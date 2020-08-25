## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd_util.c

SPD_SOURCES = empty				# 0b000
SPD_SOURCES += samsung_ddr4_4GB			# 0b001 Dual Channel 4GB
SPD_SOURCES += samsung_lpddr4_8GB		# 0b001 Dual Channel 8GB
SPD_SOURCES += empty				# 0b011
SPD_SOURCES += empty				# 0b100
SPD_SOURCES += empty				# 0b101
SPD_SOURCES += empty				# 0b110
SPD_SOURCES += empty				# 0b111
