ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SMM),y)

romstage-y += smm_helper.c
postcar-y += smm_helper.c
ramstage-y += finalize.c
ramstage-y += smm_relocate.c
ramstage-y += smm_helper.c
smm-y += smi_apmc.c
smm-y += smi_handler.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_SMM
