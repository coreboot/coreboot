/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <cpu/intel/turbo.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <option.h>
#include <static.h>
#include <types.h>
#include <variants.h>

void devtree_update(void)
{
	config_t *cfg = config_of_soc();

	struct soc_power_limits_config *soc_conf = &cfg->power_limits_config;

	struct device *nic_dev = pcidev_on_root(0x1c, 5);

	uint8_t performance_scale = 100;

	/* Update PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		performance_scale -= 25;
		cfg->tcc_offset			= 20;
		break;
	case PP_BALANCED:
		/* Use the Intel defaults */
		cfg->tcc_offset			= 15;
		break;
	case PP_PERFORMANCE:
		performance_scale += 25;
		cfg->tcc_offset			= 10;
		break;
	}

	soc_conf->tdp_pl1_override = (soc_conf->tdp_pl1_override * performance_scale) / 100;
	soc_conf->tdp_pl2_override = (soc_conf->tdp_pl2_override * performance_scale) / 100;

	/* Set PL4 to 1.0C */
	soc_conf->tdp_pl4			= 45;

	/* Enable/Disable Wireless based on CMOS settings */
	if (get_uint_option("wireless", 1) == 0)
		nic_dev->enabled = 0;

	/* Enable/Disable Webcam based on CMOS settings */
	cfg->usb2_ports[CONFIG_CCD_PORT].enable = get_uint_option("webcam", 1);

	/* Enable/Disable Card Reader based on CMOS Settings */
	if (get_uint_option("card_reader", 1) == 0)
		cfg->usb2_ports[6].enable = 0;
}
