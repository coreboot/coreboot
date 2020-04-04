/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <device/device.h>
#include <soc/pci_devs.h>
#include "chip.h"

const config_t *get_soc_config(void)
{
	const struct device *dev = pcidev_path_on_root(GNB_DEVFN);

	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "%s: Could not find SoC devicetree config!\n",
			__func__);
		return NULL;
	}

	return dev->chip_info;
}
