subdirs-y += ./*

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PCIE) += pcie.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PCIE) += pcie_rp.c
