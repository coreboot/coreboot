/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/pci_ids.h>

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 5000, 30000, 60000, 60000, 90000 },
};

void variant_devtree_update(void)
{
	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_power_limits(limits, total_entries);
}
