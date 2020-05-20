# SPDX-License-Identifier: GPL-2.0-or-later

# This directory
SPD_SOURCES_DIR := src/mainboard/$(MAINBOARDDIR)/spd

# Ordered List of APCB entries, up to 16.
# Entries should match this pattern {NAME}_x{1,2}
# There should be a matching SPD hex file in SPD_SOURCES_DIR
# matching the pattern {NAME}.spd.hex
# The _x{1,2} suffix denotes single or dual channel
# TODO: Remove channel suffix when b:141434940 is fixed
# Alternatively, generated APCBs stored at
# 3rdparty/blobs/mainboard/$(MAINBOARDDIR)/APCB_{NAME}.bin will be included.
APCB_SOURCES =  hynix-H5AN8G6NCJR-VKC_x1	# 0b0000
APCB_SOURCES += hynix-H5ANAG6NCMR-VKC_x2	# 0b0001
APCB_SOURCES += empty				# 0b0010
APCB_SOURCES += empty				# 0b0011
APCB_SOURCES += empty				# 0b0100
APCB_SOURCES += empty				# 0b0101
APCB_SOURCES += empty				# 0b0110
APCB_SOURCES += empty				# 0b0111
APCB_SOURCES += empty				# 0b1000
APCB_SOURCES += empty				# 0b1001
APCB_SOURCES += empty				# 0b1010
APCB_SOURCES += empty				# 0b1011
APCB_SOURCES += empty				# 0b1100
APCB_SOURCES += empty				# 0b1101
APCB_SOURCES += empty				# 0b1110
APCB_SOURCES += empty				# 0b1111
