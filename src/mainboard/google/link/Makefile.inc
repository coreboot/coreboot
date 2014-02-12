##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 Google Inc.
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

ramstage-y += ec.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += i915.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += i915io.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += intel_dp.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += mainboard_smi.c
SPD_BIN = $(obj)/spd.bin

# Order of names in SPD_SOURCES is important!
SPD_SOURCES = elpida_4Gb_1600_x16
SPD_SOURCES += samsung_4Gb_1600_1.35v_x16
SPD_SOURCES += micron_4Gb_1600_1.35v_x16

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/$(f).spd.hex)

# Include spd rom data
$(SPD_BIN): $(SPD_DEPS)
	for f in $^; do for c in $$(cat $$f); do echo -e -n "\\x$$c"; done; done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := 0xab
spd.bin-position := 0xfffec000
