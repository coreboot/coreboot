##
## This file is part of the coreboot project.
##
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

SPD_BIN = $(obj)/spd.bin

# Order matters for SPD sources. The following indices
# define the SPD data to use.
# RAM_ID	Vendor	Vendor_PN	Freq	Size	Total_size	channel
# 0b0011	Hynix	H5TC4G63AFR-PBA	1600MHZ	4Gb	2GB	single-channel
# 0b0100	Hynix	H5TC4G63CFR-PBA	1600MHZ	4Gb	2GB	single-channel
# 0b0101	Samsung	K4B4G1646Q-HYK0	1600MHZ	4Gb	2GB	single-channel
# 0b0110	Hynix	H5TC4G63CFR-PBA	1600MHZ	4Gb	4GB	dual-channel
# 0b0111	Samsung	K4B4G1646Q-HYK0	1600MHZ	4Gb	4GB	dual-channel
SPD_SOURCES = empty					# 0b0000
SPD_SOURCES += empty					# 0b0001
SPD_SOURCES += empty					# 0b0010
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63AFR-PBA		# 0b0011
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63CFR-PBA		# 0b0100
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0	# 0b0101
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63CFR-PBA		# 0b0110
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0	# 0b0111
SPD_SOURCES += empty					# 0b1000
SPD_SOURCES += empty					# 0b1001
SPD_SOURCES += empty					# 0b1010
SPD_SOURCES += empty					# 0b1011
SPD_SOURCES += empty					# 0b1100
SPD_SOURCES += empty					# 0b1101
SPD_SOURCES += empty					# 0b1110
SPD_SOURCES += empty					# 0b1111

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)

# Include spd ROM data
$(SPD_BIN): $(SPD_DEPS)
	for f in $+; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do printf $$(printf '\%o' 0x$$c); \
	  done; \
	done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := spd
