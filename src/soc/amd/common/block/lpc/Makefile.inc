ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_LPC) += lpc.c

bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_LPC) += lpc_util.c
verstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_LPC) += lpc_util.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_LPC) += lpc_util.c
postcar-$(CONFIG_SOC_AMD_COMMON_BLOCK_LPC) += lpc_util.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_LPC) += lpc_util.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_LPC) += lpc_util.c

bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_USE_ESPI) += espi_util.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_USE_ESPI) += espi_util.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_USE_ESPI) += espi_util.c
verstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_USE_ESPI) += espi_util.c
