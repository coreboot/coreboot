/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "cs5536.h"

#define IDE_CFG	  0x40
	#define CHANEN  (1L <<  1)
	#define PWB	(1L << 14)
	#define CABLE	(1L << 16)
#define IDE_DTC	  0x48
#define IDE_CAST  0x4C
#define IDE_ETC	  0x50

static void ide_init(struct device *dev)
{
	uint32_t ide_cfg;

	printk(BIOS_SPEW, "cs5536_ide: %s\n", __func__);
	/* GPIO and IRQ setup are handled in the main chipset code. */

	// Enable the channel and Post Write Buffer
	// NOTE: Only 32-bit writes to the data buffer are allowed when PWB is set
	ide_cfg = pci_read_config32(dev, IDE_CFG);
	ide_cfg |= CHANEN | PWB;
	pci_write_config32(dev, IDE_CFG, ide_cfg);
}

static struct device_operations ide_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = ide_init,
	.enable = 0,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops = &ide_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_CS5536_B0_IDE,
};
