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