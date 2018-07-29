#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Google Inc.
# Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
# Copyright (C) 2016 Siemens AG
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

ifeq ($(CONFIG_SOC_INTEL_FSP_BAYTRAIL),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-$(CONFIG_HAVE_SMI_HANDLER) += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/cache
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../lib/fsp
subdirs-y += fsp

ramstage-y += memmap.c
romstage-y += memmap.c
ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c
ramstage-y += spi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += spi.c
ramstage-y += chip.c
ramstage-y += iosf.c
romstage-y += iosf.c
ramstage-y += northcluster.c
ramstage-y += ramstage.c
ramstage-y += gpio.c
romstage-y += gpio.c
romstage-y += pmutil.c
ramstage-y += pmutil.c
ramstage-y += southcluster.c
romstage-y += reset.c
ramstage-y += reset.c
ramstage-y += cpu.c
ramstage-y += acpi.c
ramstage-y += lpe.c
ramstage-y += lpss.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += pmutil.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smm.c

ramstage-y += placeholders.c
ramstage-y += i2c.c
ramstage-y += gfx.c

CPPFLAGS_common += -I$(src)/soc/intel/fsp_baytrail/include
CPPFLAGS_common += -I$(src)/soc/intel/fsp_baytrail/fsp

endif
