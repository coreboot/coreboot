## SPDX-License-Identifier: GPL-2.0-only
bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_PM) += pmlib.c

romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PM_CHIPSET_STATE_SAVE) += chipset_state.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PM_CHIPSET_STATE_SAVE) += chipset_state.c
