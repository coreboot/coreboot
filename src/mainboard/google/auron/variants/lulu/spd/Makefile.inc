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

romstage-y += spd.c

SPD_BIN = $(obj)/spd.bin

# { GPIO47, GPIO9, GPIO13, GPIO8}
SPD_SOURCES  = micron_2GiB_dimm_MT41K256M16HA-125	# 0b0000 2GB
SPD_SOURCES += empty					# 0b0001
SPD_SOURCES += empty					# 0b0010
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0	# 0b0011 2GB
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646E-BYK0	# 0b0100 2GB
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646E-BYK0	# 0b0101 4GB
SPD_SOURCES += empty					# 0b0110
SPD_SOURCES += empty					# 0b0111
SPD_SOURCES += micron_2GiB_dimm_MT41K256M16HA-125	# 0b1000 4GB
SPD_SOURCES += micron_4GiB_dimm_MT41K512M16TNA-125	# 0b1001 8GB
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63AFR-PBA		# 0b1010 2GB
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63AFR-PBA		# 0b1011 4GB
SPD_SOURCES += hynix_4GiB_dimm_H5TC8G63CMR-PBA		# 0b1100 8GB
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0	# 0b1101 4GB
SPD_SOURCES += samsung_4GiB_dimm_K4B8G1646Q-MYK0	# 0b1110 8GB
SPD_SOURCES += empty					# 0b1111

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/spd/$(f).spd.hex)

# Include spd rom data
$(SPD_BIN): $(SPD_DEPS)
	for f in $+; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do printf $$(printf '\\%o' 0x$$c); \
	  done; \
	done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := spd
