## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SMN),y)

bootblock-y += smn.c
romstage-y += smn.c
ramstage-y += smn.c
smm-y += smn.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_SMN
