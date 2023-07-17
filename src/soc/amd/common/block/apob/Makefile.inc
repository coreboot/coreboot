## SPDX-License-Identifier: GPL-2.0-only
ifneq ($(CONFIG_SOC_AMD_COMMON_BLOCK_APOB_NV_DISABLE),y)
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_APOB) += apob_cache.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_APOB) += apob_cache.c

endif # !CONFIG_SOC_AMD_COMMON_BLOCK_APOB_NV_DISABLE
