## SPDX-License-Identifier: GPL-2.0-only
subdirs-y += ./*

romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PCIE) += pcie_helpers.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PCIE) += pcie.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PCIE) += pcie_helpers.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PCIE) += pcie_rp.c
