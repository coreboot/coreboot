##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Alexander Couzens <lynxis@fe80.eu>
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

SPD_SOURCES  = elpida.hex    # 0b0000 Single Channel 2GB
SPD_SOURCES  += hynix.hex    # 0b0001 2GiB
SPD_SOURCES  += samsung.hex  # 0b0010 4GiB
SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f))

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
