/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/pci_ids.h>

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 12000, 15000, 40000, 40000, 105000 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 12000, 15000, 40000, 40000, 105000 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 18000, 20000, 43000, 43000, 105000 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, 18000, 20000, 43000, 43000, 105000 },
	{ PCI_DID_INTEL_RPL_P_ID_3, 15, 12000, 15000, 40000, 40000, 114000 },
	{ PCI_DID_INTEL_RPL_P_ID_4, 15, 12000, 15000, 40000, 40000, 114000 },
};

void variant_devtree_update(void)
{
	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_power_limits(limits, total_entries);
}
