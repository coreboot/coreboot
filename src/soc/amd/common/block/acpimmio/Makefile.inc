bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c
verstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c
postcar-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += mmio_util.c

bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
verstage_x86-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
postcar-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO) += biosram.c
