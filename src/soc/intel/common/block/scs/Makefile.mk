## SPDX-License-Identifier: GPL-2.0-only
ifneq ($(CONFIG_SOC_INTEL_ALDERLAKE_PCH_N),y)
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SCS) += sd.c
endif
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SCS) += mmc.c
romstage-$(CONFIG_SOC_INTEL_COMMON_EARLY_MMC_WAKE) += early_mmc.c
