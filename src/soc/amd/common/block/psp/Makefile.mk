## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP),y)

romstage-y += psp.c
ramstage-y += psp.c
smm-y += psp.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SMI) += psp_smi.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SMI) += psp_smi_flash.c
smm-y += psp_smm.c

bootblock-y += psp_efs.c
verstage-y += psp_efs.c

all-y += ftpm.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1),y)

romstage-y += psp_gen1.c
ramstage-y += psp_gen1.c

smm-y += psp_gen1.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SMI) += psp_smi_flash_gen1.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2),y)

romstage-y += psp_gen2.c
ramstage-y += psp_gen2.c
ramstage-$(CONFIG_PSP_PLATFORM_SECURE_BOOT) += psb.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_I2C3_TPM_SHARED_WITH_PSP) += tpm.c

smm-y += psp_gen2.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SMI) += psp_smi_flash_gen2.c
smm-y += psp_smm_gen2.c

ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_RPMC) += rpmc.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SPL) += spl_fuse.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2
