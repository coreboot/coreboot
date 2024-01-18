## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SMI),y)

bootblock-y += smi_util.c
romstage-y += smi_util.c
ramstage-y += smi_util.c
smm-y += smi_util.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_SMI
