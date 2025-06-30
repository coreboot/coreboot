/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/power_limit.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/*Enable Type C port1 convert to Type A*/
	params->EnableTcssCovTypeA[1] = 1;
	/* PCH xchi port number for Type C port1 port mapping */
	params->MappingPchXhciUsbA[1] = 2;

}

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	{ PCI_DID_INTEL_ADL_N_ID_1, 7, 3000, 6000,  25000,  25000, 78000 },
	{ PCI_DID_INTEL_ADL_N_ID_2, 6, 3000, 6000,  25000,  25000, 78000 },
	{ PCI_DID_INTEL_ADL_N_ID_3, 6, 3000, 6000,  25000,  25000, 78000 },
};

const struct system_power_limits sys_limits[] = {
	/* SKU_ID, TDP (Watts), psys_pl2 (Watts) */
	{ PCI_DID_INTEL_ADL_N_ID_1, 7, 63 },
	{ PCI_DID_INTEL_ADL_N_ID_2, 6, 63 },
	{ PCI_DID_INTEL_ADL_N_ID_3, 6, 63 },
};

/*
 * Psys_pmax considerations.
 *
 * Given the hardware design in dirks, the serial shunt resistor is 0.01ohm.
 * The full scale of hardware PSYS signal 1.6v maps to system current 6.009A
 * instead of real system power. The equation is shown below:
 * PSYS = 1.6v ~= (0.01ohm x 6.009A) x 50 (INA213, gain 50V/V) x PR222/(PR222 + R3193)
 * PR222/(PR222 + R3193) = 0.5325 = 36K / (36K + 31.6K)
 *
 * The Psys_pmax is a SW setting which tells IMVP9.1 the mapping between system input
 * current and the actual system power. Since there is no voltage information
 * from PSYS, different voltage input would map to different Psys_pmax settings:
 * For Type-C 15V, the Psys_pmax should be 15v x 6.009A = 90.135W
 * For Type-C 20V, the Psys_pmax should be 20v x 6.009A = 120.18W
 * For a barrel jack, the Psys_pmax should be 19v x 6.009A = 114.171W
 *
 * Imagine that there is a type-c 100W (20V/5A) connected to DUT w/ full loading,
 * and the Psys_pmax setting is 120W. Then IMVP9.1 can calculate the current system
 * power = 120W * 5A / 6.009A = 100W, which is the actual system power.
 */
const struct psys_config psys_config = {
	.efficiency = 97,
	.psys_imax_ma = 6009,
	.bj_volts_mv = 19000,
};

void variant_devtree_update(void)
{
	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_psys_power_limits(limits, sys_limits, total_entries, &psys_config);
}
