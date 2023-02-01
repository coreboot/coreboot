## SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_PM) += pmlib.c
bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_RESET) += reset.c

verstage_x86-$(CONFIG_SOC_AMD_COMMON_BLOCK_RESET) += reset.c

romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PM_CHIPSET_STATE_SAVE) += chipset_state.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_RESET) += reset.c

ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_RESET) += reset.c

smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PM_CHIPSET_STATE_SAVE) += chipset_state.c
