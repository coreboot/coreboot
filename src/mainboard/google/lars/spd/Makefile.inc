##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 Google Inc.
## Copyright (C) 2015 Intel Corporation.
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc.
##

romstage-y += spd.c

SPD_BIN = $(obj)/spd.bin

SPD_SOURCES  = hynix_dimm_H9CCNNN8JTBLAR-NUD-1G-1866    # 0b0000
SPD_SOURCES += hynix_dimm_H9CCNNNBLTBLAR-NUD-2G-1866    # 0b0001
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF-1G-1866     # 0b0010
SPD_SOURCES += samsung_dimm_K4E6E304EB-EGCF-2G-1866     # 0b0011
SPD_SOURCES += empty                                    # 0b0100
SPD_SOURCES += empty                                    # 0b0101
SPD_SOURCES += empty                                    # 0b0110
SPD_SOURCES += empty                                    # 0b0111
SPD_SOURCES += empty                                    # 0b1000
SPD_SOURCES += empty                                    # 0b1001
SPD_SOURCES += empty                                    # 0b1010
SPD_SOURCES += empty                                    # 0b1011
SPD_SOURCES += empty                                    # 0b1100
SPD_SOURCES += empty                                    # 0b1101
SPD_SOURCES += empty                                    # 0b1110
SPD_SOURCES += empty                                    # 0b1111


SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)

# Include spd rom data
$(SPD_BIN): $(SPD_DEPS)
	for f in $+; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do printf $$(printf '\%o' 0x$$c); \
	  done; \
	done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := spd
