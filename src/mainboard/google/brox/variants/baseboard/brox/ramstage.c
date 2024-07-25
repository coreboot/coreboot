/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <drivers/intel/dptf/chip.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/power_limit.h>
#include <soc/pci_devs.h>


WEAK_DEV_PTR(dptf_policy);

#define SET_PSYSPL2(e, w) ((e) * (w) / 100)
#define SET_PL2(e, w) ((e - 27) * (w) / 100)

static bool get_sku_index(const struct cpu_power_limits *limits, size_t num_entries,
					size_t *intel_idx, size_t *brox_idx)
{
	uint16_t mchid = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);
	u8 tdp = get_cpu_tdp();
	size_t i = 0;

	for (i = 0; i < ARRAY_SIZE(cpuid_to_adl); i++) {
		if (mchid == cpuid_to_adl[i].cpu_id && tdp == cpuid_to_adl[i].cpu_tdp) {
			*intel_idx = cpuid_to_adl[i].limits;
			break;
		}
	}

	if (i == ARRAY_SIZE(cpuid_to_adl)) {
		printk(BIOS_ERR, "Cannot find correct intel sku index (mchid = %u).\n", mchid);
		return false;
	}

	for (i = 0; i < num_entries; i++) {
		if (mchid == limits[i].mchid && tdp == limits[i].cpu_tdp) {
			*brox_idx = i;
			break;
		}
	}

	if (i == num_entries) {
		printk(BIOS_ERR, "Cannot find correct brox sku index (mchid = %u).\n", mchid);
		return false;
	}

	return true;
}

void variant_update_power_limits(const struct cpu_power_limits *limits, size_t num_entries)
{
	const struct device *policy_dev;
	size_t intel_idx, brox_idx;
	struct drivers_intel_dptf_config *config;
	struct dptf_power_limits *settings;
	config_t *conf;
	struct soc_power_limits_config *soc_config;

	if (!num_entries)
		return;

	policy_dev = DEV_PTR(dptf_policy);
	if (!policy_dev)
		return;

	if (!get_sku_index(limits, num_entries, &intel_idx, &brox_idx))
		return;

	config = policy_dev->chip_info;
	settings = &config->controls.power_limits;
	conf = config_of_soc();
	soc_config = &conf->power_limits_config[intel_idx];
	settings->pl1.min_power = limits[brox_idx].pl1_min_power;
	settings->pl1.max_power = limits[brox_idx].pl1_max_power;
	settings->pl2.min_power = limits[brox_idx].pl2_min_power;
	settings->pl2.max_power = limits[brox_idx].pl2_max_power;

	if (soc_config->tdp_pl2_override != 0) {
		settings->pl2.max_power = soc_config->tdp_pl2_override * 1000;
		settings->pl2.min_power = settings->pl2.max_power;
	}

	if (soc_config->tdp_pl4 == 0)
		soc_config->tdp_pl4 = DIV_ROUND_UP(limits[brox_idx].pl4_power,
						MILLIWATTS_TO_WATTS);
}

/*
 * Psys calculations
 *
 * We use the following:
 *
 * For USB C charger (Max Power):
 * +-------------+-----+------+---------+-------+
 * | Max Power(W)| TDP | PL2  | PsysPL2 | PL3/4 |
 * +-------------+-----+------+---------+-------+
 * |     30      |  15 |  17  |   25    |  25   | <--- not working yet
 * |     45      |  15 |  26  |   38    |  38   |
 * |     60      |  15 |  35  |   51    |  51   |
 * |    110      |  15 |  55  |   94    |  96   |
 * +-------------+-----+------+---------+-------+
 */
void variant_update_psys_power_limits(const struct cpu_power_limits *limits,
				      const struct system_power_limits *sys_limits,
				      size_t num_entries,
				      const struct psys_config *config_psys)
{
	struct soc_power_limits_config *soc_config;
	size_t intel_idx, brox_idx;
	u16 volts_mv, current_ma;
	enum usb_chg_type type;
	u32 pl2;
	u32 psyspl2 = 0;
	u32 psyspl3 = 0;
	u32 pl2_default;
	config_t *conf;
	u32 watts = 0;
	int rv;

	if (!num_entries)
		return;

	if (!get_sku_index(limits, num_entries, &intel_idx, &brox_idx))
		return;

	conf = config_of_soc();
	soc_config = &conf->power_limits_config[intel_idx];

	pl2_default = DIV_ROUND_UP(limits[brox_idx].pl2_max_power, MILLIWATTS_TO_WATTS);

	/* Get AC adapter power */
	rv = google_chromeec_get_usb_pd_power_info(&type, &current_ma, &volts_mv);

	if (rv == 0 && type == USB_CHG_TYPE_PD) {
		/* Detected USB-PD. Get max value of adapter */
		watts = ((u32)current_ma * volts_mv) / 1000000;
	}
	/* If battery is present and has enough charge, add discharge rate */
	if (CONFIG(EC_GOOGLE_CHROMEEC) && google_chromeec_is_battery_present_and_above_critical_threshold()) {
		watts += 65;
	}

	/* We did not detect a battery or a Type-C charger */
	if (watts == 0) {
		return;
	}

	/* set psyspl2 to efficiency% of adapter rating */
	psyspl2 = SET_PSYSPL2(config_psys->efficiency, watts);
	psyspl3 = psyspl2;
	if (watts > 60)
		psyspl3 += 2;

	/* Limit PL2 if the adapter is with lower capability */
	pl2 = (psyspl2 > pl2_default) ? pl2_default : SET_PL2(config_psys->efficiency, watts);

	/* now that we're done calculating, set everything */
	soc_config->tdp_pl2_override = pl2;
	soc_config->tdp_psyspl2 = psyspl2;
	soc_config->tdp_psyspl3 = psyspl3;
}
