##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 Google Inc.
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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c

romstage-y += chromeos.c
ramstage-y += chromeos.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

## DIMM SPD for on-board memory
SPD_BIN = $(obj)/spd.bin

# Order of names in SPD_SOURCES is important!
# { GPIO69, GPIO68, GPIO67 }
SPD_SOURCES  = empty            # 0: { 0, 0, 0 }
SPD_SOURCES += elpida_4Gb       # 1: { 0, 0, 1 }
SPD_SOURCES += empty            # 2: { 0, 1, 0 }
SPD_SOURCES += elpida_8Gb       # 3: { 0, 1, 1 }
SPD_SOURCES += empty            # 4: { 1, 0, 0 }
SPD_SOURCES += samsung_8Gb      # 5: { 1, 0, 1 }
SPD_SOURCES += samsung_4Gb      # 6: { 1, 1, 0 }
SPD_SOURCES += empty            # 7: { 1, 1, 1 }

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/$(f).spd.hex)

# Include spd rom data
$(SPD_BIN): $(SPD_DEPS)
	for f in $+; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do echo -e -n "\\x$$c"; \
	  done; \
	done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := 0xab
