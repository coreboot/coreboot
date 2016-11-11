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

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE)$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE),y)

ramstage-y += ram_calc.c
ramstage-y += northbridge.c
ramstage-y += gma.c
ramstage-$(CONFIG_SANDYBRIDGE_IVYBRIDGE_LVDS) += gma_sandybridge_lvds.c
ramstage-$(CONFIG_SANDYBRIDGE_IVYBRIDGE_LVDS) += gma_ivybridge_lvds.c

ramstage-y += acpi.c

romstage-y += ram_calc.c
ifeq ($(CONFIG_USE_NATIVE_RAMINIT),y)
romstage-y += raminit.c
romstage-y += raminit_common.c
romstage-y += ../../../device/dram/ddr3.c
else
romstage-y += raminit_mrc.c
cbfs-files-y += mrc.bin
mrc.bin-file := $(call strip_quotes,$(CONFIG_MRC_FILE))
mrc.bin-position := 0xfffa0000
mrc.bin-type := mrc
endif
romstage-y += romstage.c
romstage-y += iommu.c
romstage-y += early_init.c
romstage-y += report_platform.c
romstage-y += ../../../arch/x86/walkcbfs.S

smm-$(CONFIG_HAVE_SMI_HANDLER) += udelay.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

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
