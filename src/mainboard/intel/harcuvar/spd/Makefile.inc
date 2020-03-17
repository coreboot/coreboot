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

# Order matters for SPD sources. The following indices
# define the SPD data to use.
SPD_SOURCES = micron_4GiB_dimm_MTA9ASF51272PZ-2G1A2

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)

# Include spd rom data
$(SPD_BIN): $(SPD_DEPS)
	for f in $+; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do echo -e -n "\\x$$c"; \
	  done; \
	done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-position := $(CONFIG_SPD_LOC)
spd.bin-type := spd
