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

	struct soc_power_limits_config *soc_conf_4core =
		&cfg->power_limits_config[ADL_N_021_6W_CORE];

	struct device *nic_dev = pcidev_on_root(0x14, 3);
	struct device *touchscreen_dev = pcidev_on_root(0x15, 2);
	struct device *accelerometer_dev = pcidev_on_root(0x15, 0);

	/* Update PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		disable_turbo();
		soc_conf_4core->tdp_pl1_override	= 6;
		soc_conf_4core->tdp_pl2_override	= 10;
		common_config->pch_thermal_trip		= 30;
		break;
	case PP_BALANCED:
		soc_conf_4core->tdp_pl1_override	= 10;
		soc_conf_4core->tdp_pl2_override	= 25;
		common_config->pch_thermal_trip		= 25;
		break;
	case PP_PERFORMANCE:
		soc_conf_4core->tdp_pl1_override	= 20;
		soc_conf_4core->tdp_pl2_override	= 35;
		common_config->pch_thermal_trip		= 20;
		break;
	}

	/* Enable/Disable Bluetooth based on CMOS settings */
	if (get_uint_option("wireless", 1) == 0) {
		cfg->usb2_ports[9].enable = 0;
		nic_dev->enabled = 0;
	}

	/* Enable/Disable Webcam based on CMOS settings */
	if (get_uint_option("webcam", 1) == 0)
		cfg->usb2_ports[CONFIG_CCD_PORT].enable = 0;

	/* Enable/Disable Camera based on CMOS settings */
	if (get_uint_option("camera", 1) == 0)
		cfg->usb2_ports[5].enable = 0;

	/* Enable/Disable Touchscreen based on CMOS settings */
	if (get_uint_option("touchscreen", 1) == 0)
		touchscreen_dev->enabled = 0;

	/* Enable/Disable Accelerometer based on CMOS settings */
	if (get_uint_option("accelerometer", 1) == 0)
		accelerometer_dev->enabled = 0;
}
