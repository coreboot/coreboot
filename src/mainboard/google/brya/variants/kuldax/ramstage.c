/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <intelblocks/power_limit.h>

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 28000, 28000,  64000,  64000, 140000 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, 28000, 28000,  64000,  64000, 140000 },
};

const struct system_power_limits sys_limits[] = {
	/* SKU_ID, TDP (Watts), psys_pl2 (Watts) */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 83 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 225 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 225 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 225 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, 225 },
};

const struct system_power_limits revise_sys_limits[] = {
	/* SKU_ID, TDP (Watts), psys_pl2 (Watts) */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 225 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 225 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 225 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 225 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, 225 },
};

/*
 * Psys_pmax considerations.
 *
 * Given the hardware design in kuldax, the serial shunt resistor is 0.005ohm.
 * The full scale of hardware PSYS signal 1.6v maps to system current 11.25A
 * instead of real system power. The equation is shown below:
 * PSYS = 1.6v = (0.005ohm x 11.25A) x 50 (INA213, gain 50V/V) x R501/(R501 + R510)
 * R501/(R501 + R510) = 0.57 = 20K / (20K + 15K)
 *
 * The Psys_pmax is a SW setting which tells IMVP9.1 the mapping b/w system input
 * current and the actual system power. Since there is no voltage information
 * from PSYS, different voltage input would map to different Psys_pmax settings:
 * For Type-C 15V, the Psys_pmax should be 15v x 11.25A = 168.75W
 * For Type-C 20V, the Psys_pmax should be 20v x 11.25A = 225W
 * For a barrel jack, the Psys_pmax should be 20v x 11.25A = 225W
 *
 * Imagine that there is a type-c 100W (20V/5A) connected to DUT w/ full loading,
 * and the Psys_pmax setting is 225W. Then IMVP9.1 can calculate the current system
 * power = 225W * 5A / 11.25A = 100W, which is the actual system power.
 */
const struct psys_config psys_config = {
	.efficiency = 97,
	.psys_imax_ma = 11250,
	.bj_volts_mv = 20000
};

void variant_devtree_update(void)
{
	size_t total_entries = ARRAY_SIZE(limits);

	if (fw_config_probe(FW_CONFIG(BJ_POWER, BJ_POWER_65W)))
		variant_update_psys_power_limits(limits, sys_limits, total_entries,
							&psys_config);
	else
		variant_update_psys_power_limits(limits, revise_sys_limits,
							total_entries, &psys_config);

	variant_update_power_limits(limits, total_entries);
}
