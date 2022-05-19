/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/power_limit.h>

const struct cpu_power_limits baseline_limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 12000, 25000,  39000,  39000, 72500 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 12000, 25000,  39000,  39000, 72500 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 12000, 25000,  39000,  39000, 72500 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 28000, 28000,  64000,  64000,  90000 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, 28000, 28000,  64000,  64000, 140000 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 45, 45000, 45000,  95000,  95000, 125000 },
	{ PCI_DID_INTEL_ADL_P_ID_4, 45, 45000, 45000, 115000, 115000, 215000 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 45, 45000, 45000, 115000, 115000, 215000 },
	{ PCI_DID_INTEL_ADL_P_ID_1, 45, 45000, 45000,  95000,  95000, 125000 },
};

const struct cpu_power_limits perf_limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 15000, 30000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 15000, 30000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 15000, 30000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 28000, 28000,  64000,  64000,  90000 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, 28000, 28000,  64000,  64000, 140000 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 45, 45000, 45000,  95000,  95000, 125000 },
	{ PCI_DID_INTEL_ADL_P_ID_4, 45, 45000, 45000, 115000, 115000, 215000 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 45, 45000, 45000, 115000, 115000, 215000 },
	{ PCI_DID_INTEL_ADL_P_ID_1, 45, 45000, 45000,  95000,  95000, 125000 },
};

const struct system_power_limits sys_limits[] = {
	/* SKU_ID, TDP (Watts), psys_pl2 (Watts) */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 65 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 230 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, 230 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 45, 230 },
	{ PCI_DID_INTEL_ADL_P_ID_4, 45, 230 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 45, 230 },
	{ PCI_DID_INTEL_ADL_P_ID_1, 45, 230 },
};

enum charger_watt {
	CHARGER_90W = 90,
	CHARGER_170W = 170,
};

/*
 * Psys_pmax considerations.
 *
 * Given the hardware design in kinox, the serial shunt resistor is 0.01ohm.
 * The full scale of hardware PSYS signal 1.6v maps to system current 5A
 * instead of real system power. The equation is shown below:
 * PSYS = 1.6v ~= (0.01ohm x 6.75A) x 50 (INA213, gain 50V/V) x R501/(R501 + R510)
 * R501/(R501 + R510) = 0.475 = 200K / (200K + 221K)
 *
 * The Psys_pmax is a SW setting which tells IMVP9.1 the mapping b/w system input
 * current and the actual system power. Since there is no voltage information
 * from PSYS, different voltage input would map to different Psys_pmax settings:
 * For Type-C 15V, the Psys_pmax should be 15v x 6.75A = 101.25W
 * For Type-C 20V, the Psys_pmax should be 20v x 6.75A = 135W
 * For a barrel jack, the Psys_pmax should be 20v x 6.75A = 135W
 *
 * Imagine that there is a type-c 100W (20V/5A) connected to DUT w/ full loading,
 * and the Psys_pmax setting is 135W. Then IMVP9.1 can calculate the current system
 * power = 135W * 5A / 6.75A = 100W, which is the actual system power.
 */
const struct psys_config psys_config = {
	.efficiency = 97,
	.psys_imax_ma = 6750,
	.bj_volts_mv = 20000
};

static void change_power_limits(const struct cpu_power_limits *limits, size_t num_entries)
{
	variant_update_psys_power_limits(limits, sys_limits, num_entries, &psys_config);
	variant_update_power_limits(limits, num_entries);
}

static void update_power_limits(void)
{
	enum usb_chg_type type;
	uint16_t volts_mv, current_ma, watts;
	size_t total_entries;
	int rv = google_chromeec_get_usb_pd_power_info(&type, &current_ma, &volts_mv);
	if (rv == 0) {
		watts = ((uint32_t)current_ma * volts_mv) / 1000000;
		printk(BIOS_INFO, "PL124: type: (%u) Current_ma: (%u) Volts_mv: (%u) Watts: (%u)\n",
			type, current_ma, volts_mv, watts);
		if (type == USB_CHG_TYPE_PROPRIETARY) {
			if (watts == CHARGER_170W) {
				printk(BIOS_INFO, "PL124: Performance.\n");
				total_entries = ARRAY_SIZE(perf_limits);
				change_power_limits(perf_limits, total_entries);
			} else {
				printk(BIOS_INFO, "PL124: Baseline.\n");
				total_entries = ARRAY_SIZE(baseline_limits);
				change_power_limits(baseline_limits, total_entries);
			}
		} else {
			if (watts >= CHARGER_90W) {
				printk(BIOS_INFO, "PL124: Performance.\n");
				total_entries = ARRAY_SIZE(perf_limits);
				change_power_limits(perf_limits, total_entries);
			} else {
				printk(BIOS_INFO, "PL124: Baseline.\n");
				total_entries = ARRAY_SIZE(baseline_limits);
				change_power_limits(baseline_limits, total_entries);
			}
		}
	} else {
		printk(BIOS_INFO, "EC cmd failure: PL124: Baseline.\n");
		total_entries = ARRAY_SIZE(baseline_limits);
		change_power_limits(baseline_limits, total_entries);
	}
}

void variant_devtree_update(void)
{
	update_power_limits();
}
