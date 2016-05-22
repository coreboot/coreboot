#
# This file is part of the coreboot project.
#
# Copyright (C) 2015-2016 Intel Corporation.
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

ifeq ($(CONFIG_SOC_INTEL_QUARK),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/x86/tsc

romstage-y += memmap.c
romstage-y += reg_access.c
romstage-y += tsc_freq.c
romstage-$(CONFIG_ENABLE_BUILTIN_HSUART1) += uart_common.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += chip.c
ramstage-y += ehci.c
ramstage-y += gpio_i2c.c
ramstage-y += memmap.c
ramstage-y += northcluster.c
ramstage-y += pmc.c
ramstage-y += reg_access.c
ramstage-y += tsc_freq.c
ramstage-$(CONFIG_ENABLE_BUILTIN_HSUART1) += uart_common.c
ramstage-$(CONFIG_ENABLE_BUILTIN_HSUART1) += uart.c

CPPFLAGS_common += -I$(src)/soc/intel/quark
CPPFLAGS_common += -I$(src)/soc/intel/quark/include

# Chipset microcode path
CPPFLAGS_common += -I3rdparty/blobs/soc/intel/quark

# Add the FSP binary to the CBFS image
cbfs-files-$(CONFIG_ADD_FSP_RAW_BIN) += fsp.bin
fsp.bin-file := $(call strip_quotes,$(CONFIG_FSP_FILE))
fsp.bin-position := $(CONFIG_FSP_LOC)
fsp.bin-type := raw

# Add the platform data file to the CBFS image
cbfs-files-$(CONFIG_ADD_FSP_PDAT_FILE) += pdat.bin
pdat.bin-file := $(call strip_quotes,$(CONFIG_FSP_PDAT_FILE))
pdat.bin-position := $(CONFIG_FSP_PDAT_LOC)
pdat.bin-type := raw

# Add the chipset microcode file to the CBFS image
cbfs-files-$(CONFIG_ADD_RMU_FILE) += rmu.bin
rmu.bin-file := $(call strip_quotes,$(CONFIG_RMU_FILE))
rmu.bin-position := $(CONFIG_RMU_LOC)
rmu.bin-type := raw

endif # CONFIG_SOC_INTEL_QUARK
