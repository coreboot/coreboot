##
## This file is part of the coreboot project.
##
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ifeq ($(CONFIG_SOC_INTEL_XEON_SP),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/cache
subdirs-$(CONFIG_HAVE_SMI_HANDLER) += ../../../cpu/x86/smm

bootblock-y += bootblock/bootblock.c
bootblock-y += lpc.c
bootblock-y += spi.c

postcar-y += soc_util.c
postcar-y += spi.c

romstage-y += soc_util.c
romstage-y += reset.c
romstage-y += romstage.c
romstage-y += soc_util.c
romstage-y += spi.c
romstage-y += hob_display.c
romstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
romstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c

ramstage-y += soc_util.c
ramstage-y += uncore.c
ramstage-y += reset.c
ramstage-y += chip.c
ramstage-y += soc_util.c
ramstage-y += lpc.c
ramstage-y += cpu.c
ramstage-y += spi.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
ramstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c
ramstage-y += hob_display.c

CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/include
CPPFLAGS_common += -I$(CONFIG_FSP_HEADER_PATH)

endif ## CONFIG_SOC_INTEL_XEON_SP
