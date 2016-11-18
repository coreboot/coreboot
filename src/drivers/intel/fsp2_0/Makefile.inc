#
# This file is part of the coreboot project.
#
# Copyright (C) 2015-2016 Intel Corp.
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

ifeq ($(CONFIG_PLATFORM_USES_FSP2_0),y)

romstage-y += debug.c
romstage-y += hand_off_block.c
romstage-$(CONFIG_DISPLAY_FSP_HEADER) += header_display.c
romstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c
romstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
romstage-$(CONFIG_VERIFY_HOBS) += hob_verify.c
romstage-y += util.c
romstage-y += memory_init.c
romstage-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c
romstage-$(CONFIG_MMA) += mma_core.c

ramstage-y += debug.c
ramstage-y += graphics.c
ramstage-y += hand_off_block.c
ramstage-$(CONFIG_DISPLAY_FSP_HEADER) += header_display.c
ramstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c
ramstage-$(CONFIG_VERIFY_HOBS) += hob_verify.c
ramstage-y += notify.c
ramstage-y += silicon_init.c
ramstage-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c
ramstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
ramstage-y += util.c
ramstage-$(CONFIG_MMA) += mma_core.c

postcar-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c

CPPFLAGS_common += -I$(src)/drivers/intel/fsp2_0/include

# Add FSP blobs into cbfs. SoC code may supply  additional options with
# -options, e.g --xip or -b
cbfs-files-$(CONFIG_ADD_FSP_BINARIES) += $(CONFIG_FSP_M_CBFS)
$(CONFIG_FSP_M_CBFS)-file := $(call strip_quotes,$(CONFIG_FSP_M_FILE))
$(CONFIG_FSP_M_CBFS)-type := fsp
ifeq ($(CONFIG_FSP_M_XIP),y)
$(CONFIG_FSP_M_CBFS)-options := --xip
endif

cbfs-files-$(CONFIG_ADD_FSP_BINARIES) += $(CONFIG_FSP_S_CBFS)
$(CONFIG_FSP_S_CBFS)-file := $(call strip_quotes,$(CONFIG_FSP_S_FILE))
$(CONFIG_FSP_S_CBFS)-type := fsp

endif
