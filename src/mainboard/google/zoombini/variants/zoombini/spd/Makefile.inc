##
## This file is part of the coreboot project.
##
## Copyright 2017 Google Inc.
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

SPD_SOURCES = Hynix_H9HCNNN8KUMLHR_1GB		# 0b000
SPD_SOURCES += Micron_MT53B512M32D2_2GB		# 1b001
SPD_SOURCES += Micron_MT53B1024M32D4_4GB	# 2b010

ifeq ($(SPD_SOURCES),)
        SPD_DEPS := $(error SPD_SOURCES is not set. Variant must provide this)
else
        SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/variants/zoombini/spd/$(f).spd.hex)
endif

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
