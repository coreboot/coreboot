##
## This file is part of the coreboot project.
##
## Copyright (C) 2015 Google Inc.
## Copyright (C) 2015 Intel Corporation
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

romstage-y += variant.c
ramstage-y += variant.c
smm-y += variant.c

SPD_BIN = $(obj)/spd.bin

# SPD data by index.  No method for board identification yet
SPD_SOURCES = samsung_dimm_K4E8E304EE-EGCF      # 0b0000
SPD_SOURCES += samsung_dimm_K4E6E304EE-EGCF     # 0b0001
SPD_SOURCES += hynix_dimm_H9CCNNN8GTMLAR        # 0b0010
SPD_SOURCES += hynix_dimm_H9CCNNNBJTMLAR        # 0b0011
SPD_SOURCES += hynix_dimm_H9CCNNNCLTMLAR        # 0b0100
SPD_SOURCES += samsung_dimm_K4E6E304EB-EGCF     # 0b0101
SPD_SOURCES += samsung_dimm_K4EBE304EB-EGCF     # 0b0110
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF     # 0b0111
SPD_SOURCES += hynix_dimm_H9CCNNNBJTALAR        # 0b1000
SPD_SOURCES += hynix_dimm_H9CCNNNCLGALAR        # 0b1001
SPD_SOURCES += hynix_dimm_H9CCNNN8GTALAR        # 0b1010

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
