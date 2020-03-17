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

# { GPIO47, GPIO9, GPIO13 }
SPD_SOURCES  = Samsung_M471B5674EB0-YK0			# 0b0000
SPD_SOURCES += Hynix_HMT425S6CFR6A_H5TC4G63CFR		# 0b0001
SPD_SOURCES += empty					# 0b0010
SPD_SOURCES += empty					# 0b0011
SPD_SOURCES += Samsung_M471B5674EB0-YK0			# 0b0100
SPD_SOURCES += Hynix_HMT425S6CFR6A_H5TC4G63CFR		# 0b0101
SPD_SOURCES += empty					# 0b0110
SPD_SOURCES += empty					# 0b0111

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
