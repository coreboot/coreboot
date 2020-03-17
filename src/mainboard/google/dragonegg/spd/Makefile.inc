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

SPD_SOURCES = Hynix_H9HCNNN8KUMLHR_2GB	# 0b000
SPD_SOURCES += Hynix_H9HCNNNCPMMLHR_4GB	# 0b001
SPD_SOURCES += Micron_MT53E512M32D2NP_2GB	# 0b010
SPD_SOURCES += Micron_MT53E2G32D8QD_8GB	# 0b011

ifeq ($(SPD_SOURCES),)
	SPD_DEPS := $(error SPD_SOURCES is not set. Variant must provide this)
else
	SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)
endif

# Include spd ROM data
$(SPD_BIN): $(SPD_DEPS)
	for f in $+; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do printf $$(printf '\\%o' 0x$$c); \
	  done; \
	done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := spd
