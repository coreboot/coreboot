/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/opregion.h>
#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <intelblocks/power_limit.h>

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 28000, 28000,  64000,  64000,  90000 },
};

const struct system_power_limits sys_limits[] = {
	/* SKU_ID, TDP (Watts), psys_pl2 (Watts) */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 135 },
};

const struct psys_config psys_config = {
	.efficiency = 97,
	.psys_imax_ma = 11000,
	.bj_volts_mv = 19500
};

void variant_devtree_update(void)
{
	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_psys_power_limits(limits, sys_limits, total_entries, &psys_config);
	variant_update_power_limits(limits, total_entries);
}
