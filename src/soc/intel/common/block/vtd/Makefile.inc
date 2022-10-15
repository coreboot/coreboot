# VT-d will not be functional in bootblock and verstage yet.
# It will become available after FSP memory init.
# If coreboot does native VT-d initialization, early DMA protection
# could be enabled in bootblock already.
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_VTD) += vtd.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_VTD) += vtd.c
