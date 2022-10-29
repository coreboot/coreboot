## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_DATA_FABRIC),y)

ramstage-y += data_fabric_helper.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_DATA_FABRIC
