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

	struct device *nic_dev = pcidev_on_root(0x14, 3);

	/* Update PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		disable_turbo();
		soc_conf->tdp_pl1_override	= 15;
		soc_conf->tdp_pl2_override	= 15;
		cfg->tcc_offset			= 20;
		break;
	case PP_BALANCED:
		soc_conf->tdp_pl1_override	= 17;
		soc_conf->tdp_pl2_override	= 20;
		cfg->tcc_offset			= 15;
		break;
	case PP_PERFORMANCE:
		soc_conf->tdp_pl1_override	= 20;
		soc_conf->tdp_pl2_override	= 25;
		cfg->tcc_offset			= 10;
		break;
	}

	/* Enable/Disable Wireless based on CMOS settings */
	if (get_uint_option("wireless", 1) == 0)
		nic_dev->enabled = 0;

	/* Enable/Disable Webcam based on CMOS settings */
	if (get_uint_option("webcam", 1) == 0)
		cfg->usb2_ports[3].enable = 0;
}
