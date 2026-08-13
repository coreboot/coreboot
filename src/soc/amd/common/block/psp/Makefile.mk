## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP),y)

ramstage-y += psp.c
smm-y += psp.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SMI) += psp_smi.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SMI) += psp_smi_flash.c
smm-y += psp_smm.c

bootblock-y += psp_efs.c
verstage-y += psp_efs.c

ramstage-$(CONFIG_AMD_CRB_FTPM) += ftpm.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1),y)

romstage-y += psp.c
romstage-y += psp_gen1.c
ramstage-y += psp_gen1.c

smm-y += psp_gen1.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SMI) += psp_smi_flash_gen1.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2),y)

all_x86-$(CONFIG_PSP_AB_RECOVERY) += psp_ab_recovery.c
smm-$(CONFIG_PSP_AB_RECOVERY) += psp_ab_recovery.c

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2_EARLY_ACCESS),y)
bootblock-y += psp.c
bootblock-y += psp_gen2.c
bootblock-y += psp_gen2_smn.c
romstage-y += psp.c
romstage-y += psp_gen2.c
romstage-y += psp_gen2_smn.c
endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2_EARLY_ACCESS

ramstage-y += psp_gen2.c
ramstage-y += psp_gen2_mmio.c
ramstage-$(CONFIG_PSP_PLATFORM_SECURE_BOOT) += psb.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_I2C3_TPM_SHARED_WITH_PSP) += tpm.c

smm-y += psp_gen2.c
smm-y += psp_gen2_mmio.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SMI) += psp_smi_flash_gen2.c
smm-y += psp_smm_gen2.c

ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_RPMC) += rpmc.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SPL) += spl_fuse.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2

smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_ROM_ARMOR3) += psp_rom_armor_smm.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_ROM_ARMOR3) += psp_rom_armor.c
