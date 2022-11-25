## SPDX-License-Identifier: GPL-2.0-or-later

ifeq ($(CONFIG_XEON_SP_COMMON_BASE),y)

subdirs-$(CONFIG_SOC_INTEL_SKYLAKE_SP) += skx lbg
subdirs-$(CONFIG_SOC_INTEL_COOPERLAKE_SP) += cpx lbg

bootblock-y += bootblock.c spi.c lpc.c pch.c
romstage-y += romstage.c reset.c util.c spi.c pmutil.c memmap.c
romstage-y += ../../../cpu/intel/car/romstage.c
ramstage-y += uncore.c reset.c util.c lpc.c spi.c ramstage.c chip_common.c
ramstage-y += memmap.c pch.c lockdown.c finalize.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PMC) += pmc.c pmutil.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += nb_acpi.c acpi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.c
smm-y += smihandler.c pmutil.c
postcar-y += spi.c

subdirs-$(CONFIG_SOC_INTEL_XEON_RAS) += ras

CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/include
CPPFLAGS_common += -include $(src)/soc/intel/xeon_sp/include/soc/fsp_upd.h

endif ## XEON_SP_COMMON_BASE
