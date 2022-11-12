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

	struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	struct soc_power_limits_config *soc_conf_10core =
		&cfg->power_limits_config[ADL_P_282_482_28W_CORE];

	struct soc_power_limits_config *soc_conf_12core =
		&cfg->power_limits_config[ADL_P_682_28W_CORE];

	struct device *nic_dev = pcidev_on_root(0x1c, 4);

	/* Update PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		disable_turbo();
		soc_conf_10core->tdp_pl1_override	= 15;
		soc_conf_12core->tdp_pl1_override	= 15;
		soc_conf_10core->tdp_pl2_override	= 15;
		soc_conf_12core->tdp_pl2_override	= 15;
		common_config->pch_thermal_trip		= 20;
		break;
	case PP_BALANCED:
		soc_conf_10core->tdp_pl1_override	= 15;
		soc_conf_12core->tdp_pl1_override	= 15;
		soc_conf_10core->tdp_pl2_override	= 25;
		soc_conf_12core->tdp_pl2_override	= 25;
		common_config->pch_thermal_trip		= 15;
		break;
	case PP_PERFORMANCE:
		soc_conf_10core->tdp_pl1_override	= 28;
		soc_conf_12core->tdp_pl1_override	= 28;
		soc_conf_10core->tdp_pl2_override	= 40;
		soc_conf_12core->tdp_pl2_override	= 40;
		common_config->pch_thermal_trip		= 10;
		break;
	}

	/* Enable/Disable Wireless based on CMOS settings */
	if (get_uint_option("wireless", 1) == 0)
		nic_dev->enabled = 0;

	/* Enable/Disable Webcam based on CMOS settings */
	if (get_uint_option("webcam", 1) == 0)
		cfg->usb2_ports[CONFIG_CCD_PORT].enable = 0;
}
