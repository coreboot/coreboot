bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c
postcar-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c

bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
postcar-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO),y)
verstage-$(CONFIG_ARCH_VERSTAGE_X86_32) += biosram.c
verstage-$(CONFIG_ARCH_VERSTAGE_X86_32) += mmio_util.c
verstage-$(CONFIG_ARCH_VERSTAGE_ARM) += mmio_util_psp.c
endif
