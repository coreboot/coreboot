#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Google Inc.
# Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_FSP_SANDYBRIDGE)$(CONFIG_NORTHBRIDGE_INTEL_FSP_IVYBRIDGE),y)

subdirs-y += fsp
ramstage-y += northbridge.c
ramstage-y += ram_calc.c
ramstage-y += gma.c

ramstage-y += acpi.c

romstage-y += raminit.c
romstage-y += ram_calc.c
romstage-y += early_init.c
romstage-y += report_platform.c
romstage-y += ../../../arch/x86/walkcbfs.S

smm-$(CONFIG_HAVE_SMI_HANDLER) += udelay.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

CPPFLAGS_common += -I$(src)/northbridge/intel/fsp_sandybridge/fsp

endif
