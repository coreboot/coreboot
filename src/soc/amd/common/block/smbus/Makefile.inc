## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SMBUS),y)

bootblock-y += smbus_early_fch.c
romstage-y += smbus.c
ramstage-y += smbus.c
ramstage-y += sm.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_SMBUS
