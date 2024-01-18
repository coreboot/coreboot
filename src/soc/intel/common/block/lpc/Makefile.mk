## SPDX-License-Identifier: GPL-2.0-only
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_LPC) += lpc_lib.c

romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_LPC) += lpc_lib.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_LPC) += lpc_lib.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_LPC) += lpc.c
