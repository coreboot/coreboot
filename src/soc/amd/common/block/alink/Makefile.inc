## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ALINK),y)

all_x86-y += alink.c
smm-y += alink.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ALINK
