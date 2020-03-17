bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP) += psp.c
bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1) += psp_gen1.c
bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2) += psp_gen2.c

romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP) += psp.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1) += psp_gen1.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2) += psp_gen2.c

ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP) += psp.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1) += psp_gen1.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2) += psp_gen2.c

smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP) += psp.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP) += psp_smm.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN1) += psp_gen1.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2) += psp_gen2.c
