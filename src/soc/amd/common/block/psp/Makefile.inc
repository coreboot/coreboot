ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP),y)

romstage-y += psp.c
ramstage-y += psp.c
smm-y += psp.c
smm-y += psp_smm.c

bootblock-y += psp_efs.c
verstage-y += psp_efs.c

ifeq ($(CONFIG_AMD_SOC_SEPARATE_EFS_SECTION),y)
bootblock-y += efs_fmap_check.c
$(call src-to-obj,bootblock,$(dir)/efs_fmap_check.c) : $(obj)/fmap_config.h
endif # CONFIG_AMD_SOC_SEPARATE_EFS_SECTION

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1),y)

romstage-y += psp_gen1.c
ramstage-y += psp_gen1.c
smm-y += psp_gen1.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2),y)

romstage-y += psp_gen2.c
ramstage-y += psp_gen2.c
ramstage-$(CONFIG_PSP_PLATFORM_SECURE_BOOT) += psb.c

smm-y += psp_gen2.c
smm-y += psp_smm_gen2.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2
