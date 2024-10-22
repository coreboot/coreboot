/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/turbo.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <option.h>
#include <static.h>
#include <types.h>
#include <variants.h>

#include "soc/intel/apollolake/chip.h"

enum cmos_power_profile get_power_profile(enum cmos_power_profile fallback)
{
	const unsigned int power_profile = get_uint_option("power_profile", fallback);
	return power_profile < NUM_POWER_PROFILES ? power_profile : fallback;
}

void devtree_update(void)
{
	config_t *cfg = config_of_soc();

	struct soc_power_limits_config *soc_conf =
		&cfg->power_limits_config;

	struct device *nic_dev = pcidev_on_root(0x0c, 0);

	uint8_t performance_scale = 100;

	/* Set PL4 to 1.0C */
	soc_conf->tdp_pl4			= 31;

	/* Set PL1 to 50% of PL2 */
	soc_conf->tdp_pl1_override = (soc_conf->tdp_pl2_override / 2) & ~1;

	/* Scale PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		performance_scale -= 25;
		cfg->tcc_offset			= 15;
		break;
	case PP_BALANCED:
		/* Use the Intel defaults */
		cfg->tcc_offset			= 10;
		break;
	case PP_PERFORMANCE:
		performance_scale += 25;
		cfg->tcc_offset			= 5;
		break;
	}

	soc_conf->tdp_pl1_override = (soc_conf->tdp_pl1_override * performance_scale) / 100;
	soc_conf->tdp_pl2_override = (soc_conf->tdp_pl2_override * performance_scale) / 100;

	/* Enable/Disable Wireless based on CMOS settings */
	if (get_uint_option("wireless", 1) == 0)
		nic_dev->enabled = 0;

	/* Enable/Disable Webcam based on CMOS settings */
	cfg->usb2_port[CONFIG_WEBCAM_USB_PORT].enable = get_uint_option("webcam", 1);
}
