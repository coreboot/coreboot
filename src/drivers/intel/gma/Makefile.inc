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

ramstage-$(CONFIG_INTEL_DP) += intel_dp.c drm_dp_helper.c display.c
ramstage-$(CONFIG_INTEL_DDI) += intel_ddi.c
ramstage-$(CONFIG_INTEL_EDID) += edid.c vbt.c
ifeq ($(CONFIG_VGA_ROM_RUN),y)
ramstage-$(CONFIG_INTEL_INT15) += int15.c
endif
ramstage-$(CONFIG_INTEL_GMA_ACPI) += acpi.c


ifeq ($(CONFIG_MAINBOARD_USE_LIBGFXINIT),y)

$(call add-special-class,gfxinit)
gfxinit-handler = $(eval ramstage-srcs += $(1)$(2))

$(call add-special-class,gfxinit-gen)
gfxinit-gen-handler = \
	$(eval additional-dirs += $(dir $(2))) \
	$(eval ramstage-srcs += $(2)) \
	$(eval ramstage-ads-deps += $(2)) \
	$(eval ramstage-adb-deps += $(2)) \
	$(eval $(2): $(obj)/config.h)

CONFIG_GFX_GMA_DEFAULT_MMIO := 0 # dummy, will be overwritten at runtime

subdirs-y += ../../../../3rdparty/libgfxinit

ramstage-y += gma.ads
ramstage-y += gma.adb

endif # CONFIG_MAINBOARD_USE_LIBGFXINIT
