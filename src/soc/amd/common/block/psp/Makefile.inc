ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP),y)

romstage-y += psp.c
ramstage-y += psp.c
smm-y += psp.c
smm-y += psp_smm.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1),y)

romstage-y += psp_gen1.c
ramstage-y += psp_gen1.c
smm-y += psp_gen1.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2),y)

romstage-y += psp_gen2.c
ramstage-y += psp_gen2.c
smm-y += psp_gen2.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2
