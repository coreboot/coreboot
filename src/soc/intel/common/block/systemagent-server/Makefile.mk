## SPDX-License-Identifier: GPL-2.0-only
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SA_SERVER) += common.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SA_SERVER) += memmap.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SA_SERVER) += systemagent_early.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SA_SERVER) += common.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SA_SERVER) += memmap.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SA_SERVER) += systemagent.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SA_SERVER) += systemagent_early.c
