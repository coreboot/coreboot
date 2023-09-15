/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/pci_ids.h>

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	/* All values are for performance config as per document #686872 */
	{ PCI_DID_INTEL_RPL_P_ID_1, 45, 18000, 45000, 115000, 115000, 210000 },
	{ PCI_DID_INTEL_RPL_P_ID_2, 28, 10000, 28000,  64000,  64000, 126000 },
	{ PCI_DID_INTEL_RPL_P_ID_3, 15,  6000, 15000,  55000,  55000, 114000 },
};

void variant_devtree_update(void)
{
	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_power_limits(limits, total_entries);
}
