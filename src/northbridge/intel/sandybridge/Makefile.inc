#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Google Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc.
#

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE)$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE_MRC)$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE)$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE_MRC),y)

ramstage-y += ram_calc.c
ramstage-y += northbridge.c
ramstage-y += gma.c
ramstage-$(CONFIG_IVYBRIDGE_LVDS) += gma_ivybridge_lvds.c
ramstage-$(CONFIG_SANDYBRIDGE_LVDS) += gma_sandybridge_lvds.c

ramstage-y += acpi.c
ramstage-y += mrccache.c

romstage-y += ram_calc.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE_MRC) += raminit_mrc.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE_MRC) += raminit_mrc.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE) += raminit.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE) += romstage.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE) += ../../../device/dram/ddr3.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE) += raminit.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE) += romstage.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE) += ../../../device/dram/ddr3.c
romstage-y += mrccache.c
romstage-y += early_init.c
romstage-y += report_platform.c
romstage-y += ../../../arch/x86/walkcbfs.S

smm-$(CONFIG_HAVE_SMI_HANDLER) += udelay.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

# We don't ship that, but booting without it is bound to fail
cbfs-files-$(CONFIG_HAVE_MRC) += mrc.bin
mrc.bin-file := $(call strip_quotes,$(CONFIG_MRC_FILE))
mrc.bin-position := 0xfffa0000
mrc.bin-type := mrc

ifneq ($(CONFIG_CHROMEOS),y)
$(obj)/mrc.cache: $(obj)/config.h
	dd if=/dev/zero count=1 \
	bs=$(shell printf "%d" $(CONFIG_MRC_CACHE_SIZE) ) | \
	tr '\000' '\377' > $@

cbfs-files-y += mrc.cache
mrc.cache-file := $(obj)/mrc.cache
mrc.cache-align := 0x10000
mrc.cache-type := mrc_cache
endif

endif
