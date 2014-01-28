/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <reg_script.h>

#include <baytrail/pci_devs.h>
#include <baytrail/ramstage.h>
#include "chip.h"

#define CAP_OVERRIDE_LOW 0xa0
#define CAP_OVERRIDE_HIGH 0xa4
# define USE_CAP_OVERRIDES (1 << 31)

static void sd_init(device_t dev)
{
	struct soc_intel_baytrail_config *config = dev->chip_info;

	if (config == NULL)
		return;

	if (config->sdcard_cap_low != 0 || config->sdcard_cap_high != 0) {
		printk(BIOS_DEBUG, "Overriding SD Card controller caps.\n");
		pci_write_config32(dev, CAP_OVERRIDE_LOW,
		                   config->sdcard_cap_low);
		pci_write_config32(dev, CAP_OVERRIDE_HIGH,
		                   config->sdcard_cap_high | USE_CAP_OVERRIDES);
	}
}

static const struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sd_init,
	.enable			= NULL,
	.scan_bus		= NULL,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver southcluster __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.device		= SD_DEVID,
};
