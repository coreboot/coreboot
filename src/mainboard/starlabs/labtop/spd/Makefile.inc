## SPDX-License-Identifier: GPL-2.0-only

# Schematics for this platform show Samsung K4A8G165WB-BCRC devices
# which are 8Gb, 2400Mbps 512Mx16 devices.
#
# The hardware platforms used for testing are fitted with a Micron part
# which has the FBGA identifier "D9ZFW".  The identification tool at
# https://www.micron.com/support/tools-and-utilities/fbga identifies
# this as the MT40A1G16KD-062E:E.  These are 16Gb, 1Gx16 devices.
#
# We have defined both SPD options below.
SPD_SOURCES = empty_ddr4			# 0b0000
SPD_SOURCES += micron-MT40A1G16KD-062E-E	# 0b0001
SPD_SOURCES += empty_ddr4			# 0b0010
SPD_SOURCES += empty_ddr4			# 0b0011
SPD_SOURCES += empty_ddr4			# 0b0100
SPD_SOURCES += empty_ddr4			# 0b0101
SPD_SOURCES += samsung-K4A8G165WB-BCRC		# 0b0110
SPD_SOURCES += samsung-K4A8G165WB-BCRC		# 0b0111

LIB_SPD_DEPS = $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)
