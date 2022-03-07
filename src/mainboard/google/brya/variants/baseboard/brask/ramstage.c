/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <soc/pci_devs.h>

#include <drivers/intel/dptf/chip.h>
#include <intelblocks/power_limit.h>

WEAK_DEV_PTR(dptf_policy);

#define SET_PSYSPL2(e, w) ((e) * (w) / 100)

static bool get_sku_index(const struct cpu_power_limits *limits, size_t num_entries,
					size_t *intel_idx, size_t *brask_idx)
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
		printk(BIOS_ERR, "Cannot find correct intel sku index.\n");
		return false;
	}

	for (i = 0; i < num_entries; i++) {
		if (mchid == limits[i].mchid && tdp == limits[i].cpu_tdp) {
			*brask_idx = i;
			break;
		}
	}

	if (i == num_entries) {
		printk(BIOS_ERR, "Cannot find correct brask sku index.\n");
		return false;
	}

	return true;
}

void variant_update_power_limits(const struct cpu_power_limits *limits, size_t num_entries)
{
	const struct device *policy_dev;
	size_t intel_idx, brask_idx;
	struct drivers_intel_dptf_config *config;
	struct dptf_power_limits *settings;
	config_t *conf;
	struct soc_power_limits_config *soc_config;

	if (!num_entries)
		return;

	policy_dev = DEV_PTR(dptf_policy);
	if (!policy_dev)
		return;

	if (!get_sku_index(limits, num_entries, &intel_idx, &brask_idx))
		return;

	config = policy_dev->chip_info;
	settings = &config->controls.power_limits;
	conf = config_of_soc();
	soc_config = &conf->power_limits_config[intel_idx];
	settings->pl1.min_power = limits[brask_idx].pl1_min_power;
	settings->pl1.max_power = limits[brask_idx].pl1_max_power;
	settings->pl2.min_power = limits[brask_idx].pl2_min_power;
	settings->pl2.max_power = limits[brask_idx].pl2_max_power;

	if (soc_config->tdp_pl2_override != 0) {
		settings->pl2.max_power = soc_config->tdp_pl2_override * 1000;
		settings->pl2.min_power = settings->pl2.max_power;
	}

	if (soc_config->tdp_pl4 == 0)
		soc_config->tdp_pl4 = DIV_ROUND_UP(limits[brask_idx].pl4_power,
						MILLIWATTS_TO_WATTS);

	printk(BIOS_INFO, "Overriding power limits PL1(mW) (%u, %u) PL2(mW) (%u, %u) PL4 (%u)\n",
			settings->pl1.min_power,
			settings->pl1.max_power,
			settings->pl2.min_power,
			settings->pl2.max_power,
			soc_config->tdp_pl4);
}

void variant_update_psys_power_limits(const struct cpu_power_limits *limits,
					const struct system_power_limits *sys_limits,
					size_t num_entries,
					const struct psys_config *config_psys)
{
	struct soc_power_limits_config *soc_config;
	const struct device *policy_dev;
	size_t intel_idx, brask_idx;
	u16 volts_mv, current_ma;
	enum usb_chg_type type;
	u32 psyspl2, pl2;
	u32 pl2_default;
	config_t *conf;
	u32 watts;
	int rv;

	if (!num_entries)
		return;

	policy_dev = DEV_PTR(dptf_policy);
	if (!policy_dev)
		return;

	if (!get_sku_index(limits, num_entries, &intel_idx, &brask_idx))
		return;

	conf = config_of_soc();
	soc_config = &conf->power_limits_config[intel_idx];
	soc_config->tdp_pl4 = 0;

	pl2_default = DIV_ROUND_UP(limits[brask_idx].pl2_max_power, MILLIWATTS_TO_WATTS);
	rv = google_chromeec_get_usb_pd_power_info(&type, &current_ma, &volts_mv);

	if (rv == 0 && type == USB_CHG_TYPE_PD) {
		/* Detected USB-PD. Base on max value of adapter */
		watts = ((u32)current_ma * volts_mv) / 1000000;

		/* set psyspl2 to 97% of adapter rating */
		psyspl2 = SET_PSYSPL2(config_psys->efficiency, watts);

		/* Limit PL2 if the adapter is with lower capability */
		pl2 = (psyspl2 > pl2_default) ? pl2_default : psyspl2;

		soc_config->tdp_pl4 = psyspl2;
	} else {
		/* Input type is barrel jack */
		volts_mv = config_psys->bj_volts_mv;
		psyspl2 = sys_limits[brask_idx].psys_pl2_power;
		pl2 = pl2_default;
	}

	/* voltage unit is milliVolts and current is in milliAmps */
	soc_config->psys_pmax = (u16)(((u32)config_psys->psys_imax_ma * volts_mv) / 1000000);
	conf->platform_pmax = soc_config->psys_pmax;

	soc_config->tdp_pl2_override = pl2;
	soc_config->tdp_psyspl2 = psyspl2;

	printk(BIOS_INFO, "Overriding PL2 (%u) PsysPL2 (%u) Psys_Pmax (%u)\n",
			soc_config->tdp_pl2_override,
			soc_config->tdp_psyspl2,
			soc_config->psys_pmax);
}
