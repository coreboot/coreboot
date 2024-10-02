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

	struct soc_power_limits_config *soc_conf_6core =
		&cfg->power_limits_config[RPL_P_282_242_142_15W_CORE];

	struct soc_power_limits_config *soc_conf_12core =
		&cfg->power_limits_config[RPL_P_682_482_282_28W_CORE];

	struct device *tbt_pci_dev = pcidev_on_root(0x07, 0);
	struct device *tbt_dma_dev = pcidev_on_root(0x0d, 2);

	/* Update PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		soc_conf_6core->tdp_pl1_override	= 15;
		soc_conf_12core->tdp_pl1_override	= 15;
		soc_conf_6core->tdp_pl2_override	= 15;
		soc_conf_12core->tdp_pl2_override	= 15;
		common_config->pch_thermal_trip		= 30;
		break;
	case PP_BALANCED:
		soc_conf_6core->tdp_pl1_override	= 15;
		soc_conf_12core->tdp_pl1_override	= 15;
		soc_conf_6core->tdp_pl2_override	= 20;
		soc_conf_12core->tdp_pl2_override	= 25;
		common_config->pch_thermal_trip		= 25;
		break;
	case PP_PERFORMANCE:
		soc_conf_6core->tdp_pl1_override	= 15;
		soc_conf_12core->tdp_pl1_override	= 28;
		soc_conf_6core->tdp_pl2_override	= 25;
		soc_conf_12core->tdp_pl2_override	= 40;
		common_config->pch_thermal_trip		= 20;
		break;
	}

	/* Enable/Disable Bluetooth based on CMOS settings */
	if (get_uint_option("wireless", 1) == 0)
		cfg->usb2_ports[9].enable = 0;

	/* Enable/Disable Webcam based on CMOS settings */
	if (get_uint_option("webcam", 1) == 0)
		cfg->usb2_ports[CONFIG_CCD_PORT].enable = 0;

	/* Enable/Disable Thunderbolt based on CMOS settings */
	if (get_uint_option("thunderbolt", 1) == 0) {
		tbt_pci_dev->enabled = 0;
		tbt_dma_dev->enabled = 0;
	}
}
