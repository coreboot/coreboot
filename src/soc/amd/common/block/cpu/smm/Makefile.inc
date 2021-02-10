ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SMM),y)

ramstage-y += smm_relocate.c
smm-y += smi_handler.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_SMM
