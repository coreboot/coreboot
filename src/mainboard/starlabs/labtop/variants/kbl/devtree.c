/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <cpu/intel/turbo.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <option.h>
#include <types.h>
#include <variants.h>

void devtree_update(void)
{
	config_t *cfg = config_of_soc();

	struct soc_power_limits_config *soc_conf = &cfg->power_limits_config;

	struct device *nic_dev = pcidev_on_root(0x1c, 5);

	/* Update PL1 & PL2 based on CMOS settings */
	switch (get_uint_option("power_profile", 0)) {
	case 1:
		soc_conf->tdp_pl1_override = 17;
		soc_conf->tdp_pl2_override = 20;
		break;
	case 2:
		soc_conf->tdp_pl1_override = 20;
		soc_conf->tdp_pl2_override = 25;
		break;
	default:
		disable_turbo();
		soc_conf->tdp_pl1_override = 15;
		soc_conf->tdp_pl2_override = 15;
		break;
	}

	/* Enable/Disable Wireless based on CMOS settings */
	if (get_uint_option("wireless", 1) == 0)
		nic_dev->enabled = 0;

	/* Enable/Disable Webcam based on CMOS settings */
	cfg->usb2_ports[CONFIG_CCD_PORT].enable = get_uint_option("webcam", 1);
}
