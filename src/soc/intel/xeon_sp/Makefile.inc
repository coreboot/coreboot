## SPDX-License-Identifier: GPL-2.0-or-later

ifeq ($(CONFIG_XEON_SP_COMMON_BASE),y)

subdirs-$(CONFIG_SOC_INTEL_SKYLAKE_SP) += skx
subdirs-$(CONFIG_SOC_INTEL_COOPERLAKE_SP) += cpx

bootblock-y += bootblock.c spi.c lpc.c gpio.c pch.c
romstage-y += romstage.c reset.c util.c spi.c gpio.c pmutil.c memmap.c
romstage-y += ../../../cpu/intel/car/romstage.c
ramstage-y += uncore.c reset.c util.c lpc.c spi.c gpio.c nb_acpi.c ramstage.c chip_common.c
ramstage-y += memmap.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PMC) += pmc.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
postcar-y += spi.c

CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/include
CPPFLAGS_common += -I$(CONFIG_FSP_HEADER_PATH)

endif ## XEON_SP_COMMON_BASE
