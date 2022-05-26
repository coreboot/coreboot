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

	struct soc_power_limits_config *soc_conf_2core =
		&cfg->power_limits_config[POWER_LIMITS_U_2_CORE];

	struct soc_power_limits_config *soc_conf_4core =
		&cfg->power_limits_config[POWER_LIMITS_U_4_CORE];

	struct device *nic_dev = pcidev_on_root(0x14, 3);
	struct device *tbt_pci_dev = pcidev_on_root(0x07, 0);
	struct device *tbt_dma_dev = pcidev_on_root(0x0d, 2);


	/* Update PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		disable_turbo();
		soc_conf_2core->tdp_pl1_override	= 15;
		soc_conf_4core->tdp_pl1_override	= 15;
		soc_conf_2core->tdp_pl2_override	= 15;
		soc_conf_4core->tdp_pl2_override	= 15;
		cfg->tcc_offset				= 20;
		break;
	case PP_BALANCED:
		soc_conf_2core->tdp_pl1_override	= 15;
		soc_conf_4core->tdp_pl1_override	= 15;
		soc_conf_2core->tdp_pl2_override	= 25;
		soc_conf_4core->tdp_pl2_override	= 25;
		cfg->tcc_offset				= 15;
		break;
	case PP_PERFORMANCE:
		soc_conf_2core->tdp_pl1_override	= 28;
		soc_conf_4core->tdp_pl1_override	= 28;
		soc_conf_2core->tdp_pl2_override	= 40;
		soc_conf_4core->tdp_pl2_override	= 40;
		cfg->tcc_offset				= 10;
		break;
	}

	/* Enable/Disable Wireless based on CMOS settings */
	if (get_uint_option("wireless", 1) == 0)
		nic_dev->enabled = 0;

	/* Enable/Disable Webcam based on CMOS settings */
	cfg->usb2_ports[CONFIG_CCD_PORT].enable = get_uint_option("webcam", 1);

	/* Enable/Disable Thunderbolt based on CMOS settings */
	if (get_uint_option("thunderbolt", 1) == 0) {
		cfg->UsbTcPortEn = 0;
		cfg->TcssXhciEn = 0;
		cfg->TcssD3ColdDisable = 0;
		tbt_pci_dev->enabled = 0;
		tbt_dma_dev->enabled = 0;
	}
}
