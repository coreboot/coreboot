/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "hudson.h"

static void sd_init(struct device *dev)
{
	u32 stepping;

	stepping = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18, 3)), 0xFC);

	struct southbridge_amd_pi_hudson_config *sd_chip =
		(struct southbridge_amd_pi_hudson_config *)(dev->chip_info);

	if (sd_chip->sd_mode == 3) {	/* SD 3.0 mode */
		pci_write_config32(dev, 0xA4, 0x31FEC8B2);
		pci_write_config32(dev, 0xA8, 0x00002503);
		pci_write_config32(dev, 0xB0, 0x02180C19);
		pci_write_config32(dev, 0xD0, 0x0000078B);
	}
	else {				/* SD 2.0 mode */
		if ((stepping & 0x0000000F) == 0) {	/* Stepping A0 */
			pci_write_config32(dev, 0xA4, 0x31DE32B2);
			pci_write_config32(dev, 0xB0, 0x01180C19);
			pci_write_config32(dev, 0xD0, 0x0000058B);
		}
		else {					/* Stepping >= A1 */
			pci_write_config32(dev, 0xA4, 0x31FE32B2);
			pci_write_config32(dev, 0xA8, 0x00000070);
			pci_write_config32(dev, 0xB0, 0x01180C19);
			pci_write_config32(dev, 0xD0, 0x0000078B);
		}
	}
}

static struct device_operations sd_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sd_init,
	.scan_bus = 0,
};

static const struct pci_driver sd_driver __pci_driver = {
	.ops = &sd_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_YANGTZE_SD,
};
