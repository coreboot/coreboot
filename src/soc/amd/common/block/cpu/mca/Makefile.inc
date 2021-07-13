ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_MCA_COMMON),y)
ramstage-y += mca_common.c
ramstage-y += mca_common_bert.c
endif # CONFIG_SOC_AMD_COMMON_BLOCK_MCA_COMMON

ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_MCA) += mca_bert.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_MCAX) += mcax_bert.c
