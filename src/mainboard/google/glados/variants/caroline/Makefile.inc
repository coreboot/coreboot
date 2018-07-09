##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Google Inc.
## Copyright (C) 2016 Intel Corporation
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

# SPD data by index
SPD_SOURCES  = samsung_dimm_K4E8E324EB-EGCF     # 0b0000
SPD_SOURCES += samsung_dimm_K4E6E304EB-EGCF     # 0b0001
SPD_SOURCES += samsung_dimm_K4EBE304EB-EGCG     # 0b0010

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
