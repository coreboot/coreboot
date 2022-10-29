## SPDX-License-Identifier: GPL-2.0-only
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_THERMAL) += thermal_common.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_THERMAL_PCI_DEV) += thermal_pci.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_THERMAL_BEHIND_PMC) += thermal_pmc.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_THERMAL) += thermal_common.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_THERMAL_PCI_DEV) += thermal_pci.c
