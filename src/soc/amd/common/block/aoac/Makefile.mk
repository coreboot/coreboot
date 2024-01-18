## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_AOAC),y)

bootblock-y += aoac.c
romstage-y += aoac.c
verstage-y += aoac.c
ramstage-y += aoac.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_AOAC
