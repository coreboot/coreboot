## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_BOARD_STARLABS_LABTOP_KBL),y)
SPD_SOURCES = empty_ddr4			# 0b0000
SPD_SOURCES += micron-MT40A1G16KD-062E-E	# 0b0001
SPD_SOURCES += empty_ddr4			# 0b0010
SPD_SOURCES += empty_ddr4			# 0b0011
SPD_SOURCES += empty_ddr4			# 0b0100
SPD_SOURCES += empty_ddr4			# 0b0101
SPD_SOURCES += samsung-K4A8G165WB-BCRC		# 0b0110
SPD_SOURCES += samsung-K4A8G165WB-BCRC		# 0b0111
else
SPD_SOURCES = micron-MT40A1G16KD-062E-E	# 0b0000
endif

LIB_SPD_DEPS = $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)
