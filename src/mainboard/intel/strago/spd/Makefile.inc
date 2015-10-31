##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 Google Inc.
## Copyright (C) 2015 Intel Corp.
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

SPD_SOURCES = samsung_2GiB_dimm_K4B4G1646Q-HYK0
SPD_SOURCES += hynix_2GiB_dimm_HMT425S6CFR6A_H5TC4G63CFR
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0
SPD_SOURCES += hynix_2GiB_dimm_HMT425S6CFR6A_H5TC4G63CFR

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
