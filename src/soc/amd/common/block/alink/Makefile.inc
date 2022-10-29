## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ALINK),y)

bootblock-y += alink.c
verstage_x86-y += alink.c
romstage-y += alink.c
postcar-y += alink.c
ramstage-y += alink.c
smm-y += alink.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ALINK
