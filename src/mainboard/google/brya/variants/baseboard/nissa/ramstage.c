/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/ramstage.h>
#include <soc/pci_devs.h>
#include <static.h>
#include <intelblocks/power_limit.h>

#define SET_PSYSPL2(e, w) ((e) * (w) / 100)

struct soc_power_limits_config *variant_get_soc_power_limit_config(void)
{
	config_t *config = config_of_soc();
	size_t i;
	struct device *sa = pcidev_path_on_root(SA_DEVFN_ROOT);
	uint16_t sa_pci_id;
	u8 tdp;

	if (!sa)
		return NULL;

	sa_pci_id = pci_read_config16(sa, PCI_DEVICE_ID);

	if (sa_pci_id == 0xffff)
		return NULL;

	tdp = get_cpu_tdp();

	for (i = 0; i < ARRAY_SIZE(cpuid_to_adl); i++) {
		if (sa_pci_id == cpuid_to_adl[i].cpu_id &&
				tdp == cpuid_to_adl[i].cpu_tdp) {
			return &config->power_limits_config[cpuid_to_adl[i].limits];
		}
	}

	return NULL;
}

static bool get_sku_index(const struct cpu_power_limits *limits, size_t num_entries,
					size_t *intel_idx, size_t *nissa_idx)
{
	uint16_t mch_id = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);
	u8 tdp = get_cpu_tdp();
	size_t i = 0;

	for (i = 0; i < ARRAY_SIZE(cpuid_to_adl); i++) {
		if (mch_id == cpuid_to_adl[i].cpu_id && tdp == cpuid_to_adl[i].cpu_tdp) {
			*intel_idx = cpuid_to_adl[i].limits;
			break;
		}
	}

	if (i == ARRAY_SIZE(cpuid_to_adl)) {
		printk(BIOS_ERR, "Cannot find correct intel sku index.\n");
		return false;
	}

	for (i = 0; i < num_entries; i++) {
		if (mch_id == limits[i].mchid && tdp == limits[i].cpu_tdp) {
			*nissa_idx = i;
			break;
		}
	}

	if (i == num_entries) {
		printk(BIOS_ERR, "Cannot find correct nissa sku index.\n");
		return false;
	}

	return true;
}

void variant_update_psys_power_limits(const struct cpu_power_limits *limits,
					const struct system_power_limits *sys_limits,
					size_t num_entries,
					const struct psys_config *config_psys)
{
	struct soc_power_limits_config *soc_config;
	size_t intel_idx, nissa_idx;
	u16 volts_mv, current_ma;
	enum usb_chg_type type;
	u32 psyspl2;
	config_t *conf;
	u32 watts;
	int rv;

	if (!num_entries)
		return;

	if (!get_sku_index(limits, num_entries, &intel_idx, &nissa_idx))
		return;

	conf = config_of_soc();
	soc_config = &conf->power_limits_config[intel_idx];

	rv = google_chromeec_get_usb_pd_power_info(&type, &current_ma, &volts_mv);

	if (rv == 0 && type == USB_CHG_TYPE_PD) {
		/* Detected USB-PD. Base on max value of adapter */
		watts = ((u32)current_ma * volts_mv) / 1000000;

		/* set psyspl2 to 97% of adapter rating */
		psyspl2 = SET_PSYSPL2(config_psys->efficiency, watts);
	} else {
		/* Input type is barrel jack */
		volts_mv = config_psys->bj_volts_mv;
		psyspl2 = sys_limits[nissa_idx].psys_pl2_power;
	}

	/* voltage unit is milliVolts and current is in milliAmps */
	soc_config->psys_pmax = (u16)(((u32)config_psys->psys_imax_ma * volts_mv) / 1000000);
	conf->platform_pmax = soc_config->psys_pmax;

	soc_config->tdp_psyspl2 = psyspl2;

	printk(BIOS_INFO, "Overriding PsysPL2 (%u) Psys_Pmax (%u)\n",
			soc_config->tdp_psyspl2,
			soc_config->psys_pmax);
}

void variant_configure_pads(void)
{
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	struct pad_config *padbased_table;
	size_t base_num, override_num;

	padbased_table = new_padbased_table();
	base_pads = variant_gpio_table(&base_num);
	gpio_padbased_override(padbased_table, base_pads, base_num);
	override_pads = variant_gpio_override_table(&override_num);
	if (override_pads != NULL)
		gpio_padbased_override(padbased_table, override_pads, override_num);
	fw_config_gpio_padbased_override(padbased_table);
	gpio_configure_pads_with_padbased(padbased_table);
	free(padbased_table);
}
