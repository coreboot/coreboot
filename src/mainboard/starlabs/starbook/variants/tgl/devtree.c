/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <cpu/intel/turbo.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <option.h>
#include <static.h>
#include <types.h>
#include <variants.h>
#include <common/powercap.h>

#define TJ_MAX		100
#define TCC(temp)	(TJ_MAX - temp)

void devtree_update(void)
{
	config_t *cfg = config_of_soc();

	struct soc_power_limits_config *soc_conf_2core =
		&cfg->power_limits_config[POWER_LIMITS_U_2_CORE];

	struct soc_power_limits_config *soc_conf_4core =
		&cfg->power_limits_config[POWER_LIMITS_U_4_CORE];

	struct device *wifi_dev = pcidev_on_root(0x14, 3);
	struct device *tbt_pci_dev = pcidev_on_root(0x07, 0);
	struct device *tbt_dma_dev = pcidev_on_root(0x0d, 2);
	struct device *gna_dev = pcidev_on_root(0x08, 0);

	uint8_t performance_scale = 100;

	/* Set PL4 to 1.0C */
	soc_conf_2core->tdp_pl4				= 65;
	soc_conf_4core->tdp_pl4				= 65;

	/* Set PL1 to 50% of PL2 */
	soc_conf_2core->tdp_pl1_override = (soc_conf_2core->tdp_pl2_override / 2) & ~1;
	soc_conf_4core->tdp_pl1_override = (soc_conf_4core->tdp_pl2_override / 2) & ~1;

	/* Scale PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		performance_scale			-= 50;
		cfg->tcc_offset				= TCC(80);
		break;
	case PP_BALANCED:
		performance_scale			-= 25;
		cfg->tcc_offset				= TCC(90);
		break;
	case PP_PERFORMANCE:
		/* Use the Intel defaults */
		cfg->tcc_offset				= TCC(100);
		break;
	}

	soc_conf_2core->tdp_pl1_override = (soc_conf_2core->tdp_pl1_override * performance_scale) / 100;
	soc_conf_2core->tdp_pl2_override = (soc_conf_2core->tdp_pl2_override * performance_scale) / 100;

	soc_conf_4core->tdp_pl1_override = (soc_conf_4core->tdp_pl1_override * performance_scale) / 100;
	soc_conf_4core->tdp_pl2_override = (soc_conf_4core->tdp_pl2_override * performance_scale) / 100;

	/* Enable/Disable WiFi based on CMOS settings */
	if (get_uint_option("wifi", 1) == 0)
		wifi_dev->enabled = 0;

	/* Enable/Disable Bluetooth based on CMOS settings */
	if (get_uint_option("bluetooth", 1) == 0)
		cfg->usb2_ports[9].enable = 0;

	/* Enable/Disable Webcam based on CMOS settings */
	cfg->usb2_ports[CONFIG_CCD_PORT].enable = get_uint_option("webcam", 1);

	/* Enable/Disable Card Reader based on CMOS Settings */
	if (get_uint_option("card_reader", 1) == 0)
		cfg->usb2_ports[5].enable = 0;

	/* Enable/Disable Thunderbolt based on CMOS settings */
	if (get_uint_option("thunderbolt", 1) == 0) {
		cfg->UsbTcPortEn = 0;
		cfg->TcssXhciEn = 0;
		tbt_pci_dev->enabled = 0;
		tbt_dma_dev->enabled = 0;
	}

	/* Enable/Disable GNA based on CMOS settings */
	if (get_uint_option("gna", 0) == 0)
		gna_dev->enabled = 0;
}
